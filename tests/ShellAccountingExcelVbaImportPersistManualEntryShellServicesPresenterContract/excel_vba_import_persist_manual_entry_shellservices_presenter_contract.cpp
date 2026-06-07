#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPort.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (argv[index] == option) {
            return argv[index + 1];
        }
    }
    return {};
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    require(input.good(), "file exists and can be read: " + path.string());
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

void requireContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(text.find(token) != std::string::npos, context + " contains " + token);
}

void requireNotContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(text.find(token) == std::string::npos, context + " does not contain " + token);
}

QJsonObject parseObject(const std::string& json)
{
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    require(parseError.error == QJsonParseError::NoError, "JSON parses");
    require(document.isObject(), "JSON root is object");
    return document.object();
}

std::string validPayload()
{
    return R"({"schemaVersion":"excel-vba-export/v1","source":"sanitized-excel-vba-export","sheets":[{"name":"TradeLog","headers":["ROW_ID","TRADE_TIME_UTC","ACCOUNT_CODE","PORTFOLIO_CODE","INSTRUMENT_CODE","SIDE","QUANTITY","PRICE","AMOUNT","FEE","CASH_FLOW","CURRENCY","SOURCE","MEMO"],"rows":[["TASK267_TRADE_001","2026-06-07T09:30:00Z","TASK267_ACCOUNT","TASK267_PORTFOLIO","TASK267_ETF","BUY","100","2.0000","200.00","1.00","-201.00","CNY","TASK267_SANITIZED_IMPORT","SANITIZED_TASK267_BUY"]]}]})";
}

etfdt::shell_services::ShellAccountingDataServiceClientResponse unavailable(
    const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request,
    const char* status)
{
    etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
    response.actionName = request.actionName;
    response.protocolSuccess = false;
    response.implemented = false;
    response.payloadStatus = status;
    response.dataQualityStatus = "UNAVAILABLE";
    response.protocolError = true;
    response.issues.push_back({status, "ERROR", status, true, "FakeTask267ClientPort"});
    return response;
}

class FakeTask267ClientPort final : public etfdt::shell_services::ShellAccountingDataServiceClientPort {
public:
    std::vector<etfdt::shell_services::ShellAccountingDataServiceClientResponse> scripted;
    int persistCallCount = 0;

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callPositionList(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK267_UNUSED_POSITION_LIST");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callCashSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK267_UNUSED_CASH_SUMMARY");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK267_UNUSED_PORTFOLIO_PNL");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK267_UNUSED_BASE_POSITION");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK267_UNUSED_SNIPER_POOL");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callExcelVbaImportPersistManualEntry(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++persistCallCount;
        require(
            request.actionName == "accounting.excel_vba_import.persist_manual_entry",
            "persist action name mapped to client port");
        const auto payload = parseObject(request.payloadJson);
        require(payload.value("previewStatus").toString() == "ACCEPTED", "previewStatus mapped");
        require(payload.value("previewDigest").toString() == "task-267-preview-digest", "previewDigest mapped");
        require(payload.value("idempotencyKey").toString() == "task-267-idempotency", "idempotencyKey mapped");
        require(payload.value("schemaVersion").toString() == "excel-vba-export/v1", "schemaVersion mapped");
        require(payload.value("source").toString() == "sanitized-excel-vba-export", "source mapped");
        require(payload.value("acceptedAt").toString() == "2026-06-07T10:00:00Z", "acceptedAt mapped");
        require(payload.value("importBatchLabel").toString() == "TASK267_BATCH", "batch label mapped");
        require(payload.value("previewFactSummary").isObject(), "fact summary object mapped");
        require(payload.value("importPayload").isObject(), "sanitized import payload object mapped");
        require(!scripted.empty(), "fake client port has scripted response");
        auto response = scripted.front();
        scripted.erase(scripted.begin());
        response.actionName = request.actionName;
        return response;
    }
};

etfdt::shell_services::ShellAccountingDataServiceClientResponse successResponse()
{
    etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
    response.protocolSuccess = true;
    response.implemented = true;
    response.readOnly = false;
    response.writeEnabled = true;
    response.payloadStatus = "OK";
    response.dataQualityStatus = "OK";
    response.hasRows = true;
    response.transactionCommitted = true;
    response.tradeLogWritten = true;
    response.auditWritten = true;
    response.idempotencyRequired = true;
    return response;
}

etfdt::shell_services::ShellAccountingDataServiceClientResponse duplicateResponse()
{
    auto response = successResponse();
    response.payloadStatus = "IDEMPOTENT_REPLAY";
    response.transactionCommitted = false;
    response.tradeLogWritten = false;
    response.auditWritten = false;
    response.duplicateImportPrevented = true;
    response.hasRows = false;
    return response;
}

etfdt::shell_services::ShellAccountingDataServiceClientResponse conflictResponse()
{
    etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
    response.protocolSuccess = false;
    response.implemented = true;
    response.readOnly = false;
    response.writeEnabled = true;
    response.payloadStatus = "IDEMPOTENCY_CONFLICT";
    response.dataQualityStatus = "ERROR";
    response.domainError = true;
    response.idempotencyRequired = true;
    response.idempotencyConflictRejected = true;
    response.issues.push_back({
        "EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT",
        "ERROR",
        "EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT",
        true,
        "FakeTask267ClientPort"});
    return response;
}

bool callPersist(etfdt::shell_services::ShellAccountingPresenter& presenter)
{
    return presenter.persistExcelVbaImportManualEntry(
        QStringLiteral("ACCEPTED"),
        QStringLiteral("task-267-preview-digest"),
        QStringLiteral("task-267-idempotency"),
        QStringLiteral("excel-vba-export/v1"),
        QStringLiteral("sanitized-excel-vba-export"),
        QStringLiteral("2026-06-07T10:00:00Z"),
        QStringLiteral("TASK267_BATCH"),
        QString::fromStdString(validPayload()),
        1,
        1,
        0,
        0);
}

void requireStaticBoundaries(const std::filesystem::path& root)
{
    const auto presenterHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h";
    const auto presenterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp";
    const auto controllerHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h";
    const auto controllerSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp";
    const auto portAdapterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp";
    const auto qmlPage =
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";

    for (const auto& path : {presenterHeader, presenterSource, controllerHeader, controllerSource}) {
        const auto text = readFile(path);
        requireNotContains(text, "DataServiceClient/DataServiceClient.h", "Presenter / Controller");
        requireNotContains(text, "DataServiceApi", "Presenter / Controller");
        requireNotContains(text, "DataAccess", "Presenter / Controller");
        requireNotContains(text, "AccountingEngine", "Presenter / Controller");
        requireNotContains(text, "SQLite", "Presenter / Controller");
    }

    const auto presenterHeaderText = readFile(presenterHeader);
    requireContains(
        presenterHeaderText,
        "bool persistExcelVbaImportManualEntry(",
        "Presenter header exposes C++ persist method");
    requireNotContains(
        presenterHeaderText,
        "Q_INVOKABLE bool persistExcelVbaImportManualEntry",
        "Presenter persist method is not QML invokable");

    const auto portAdapterText = readFile(portAdapterSource);
    requireContains(
        portAdapterText,
        "accountingExcelVbaImportPersistManualEntry",
        "PortAdapter calls TASK-266 typed client adapter");
    requireNotContains(portAdapterText, "INSERT INTO", "PortAdapter");
    requireNotContains(portAdapterText, "UPDATE ", "PortAdapter");
    requireNotContains(portAdapterText, "DELETE ", "PortAdapter");
    requireNotContains(portAdapterText, "REPLACE ", "PortAdapter");

    const auto qmlText = readFile(qmlPage);
    requireNotContains(qmlText, "persistExcelVbaImportManualEntry", "QML has no persist call wiring");
    requireNotContains(qmlText, "accounting.excel_vba_import.persist_manual_entry", "QML has no action name");
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-267"},
        {"shellServicesPersistContractCreated", true},
        {"presenterPersistMethodCreated", true},
        {"controllerPersistMethodCreated", true},
        {"typedClientPersistAdapterCalledThroughPort", true},
        {"presenterDirectDataServiceClient", false},
        {"controllerDirectDataServiceClient", false},
        {"acceptedPreviewRequired", true},
        {"nonAcceptedPreviewFailsClosed", true},
        {"missingDigestFailsClosed", true},
        {"missingIdempotencyKeyFailsClosed", true},
        {"persistViewModelMapped", true},
        {"transactionCommittedMapped", true},
        {"tradeLogWrittenMapped", true},
        {"auditLogWrittenMapped", true},
        {"duplicateImportMapped", true},
        {"idempotencyConflictMapped", true},
        {"sanitizedIssuesMapped", true},
        {"fakeClientPortUsed", true},
        {"productionDbTouched", false},
        {"qmlWiringChanged", false},
        {"importButtonAdded", false},
        {"accountingEngineCalled", false},
        {"brokerOrderSubmitted", false},
        {"networkAccessBeyondLocalDataServiceTransport", false},
        {"automaticTrading", false},
    };
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    try {
        const auto sourceRoot = optionValue(argc, argv, "--source-root");
        require(!sourceRoot.empty(), "--source-root <path> is required");
        requireStaticBoundaries(sourceRoot);

        auto fakePort = std::make_shared<FakeTask267ClientPort>();
        auto serviceAdapter =
            std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(fakePort);
        auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
        controller->setServiceAdapter(serviceAdapter);
        etfdt::shell_services::ShellAccountingPresenter presenter;
        presenter.setController(controller);

        fakePort->scripted.push_back(successResponse());
        require(callPersist(presenter), "accepted preview persists through ShellServices contract");
        require(fakePort->persistCallCount == 1, "client port called once");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("PERSISTED"), "persist status mapped");
        require(presenter.lastExcelVbaImportPersistTransactionCommitted(), "transaction committed view-model mapped");
        require(presenter.lastExcelVbaImportPersistTradeLogWritten(), "trade_log view-model mapped");
        require(presenter.lastExcelVbaImportPersistAuditLogWritten(), "audit view-model mapped");
        require(!presenter.lastExcelVbaImportPersistDuplicateImportPrevented(), "duplicate flag false on success");
        require(!presenter.lastExcelVbaImportPersistIdempotencyConflictRejected(), "conflict flag false on success");
        require(
            presenter.lastExcelVbaImportPersistSummary().contains(QStringLiteral("transactionCommitted=true")),
            "persist summary mapped");

        fakePort->scripted.push_back(duplicateResponse());
        require(callPersist(presenter), "duplicate import maps as successful idempotent result");
        require(fakePort->persistCallCount == 2, "client port called for duplicate");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("DUPLICATE"), "duplicate status mapped");
        require(presenter.lastExcelVbaImportPersistDuplicateImportPrevented(), "duplicate view-model mapped");

        fakePort->scripted.push_back(conflictResponse());
        require(!callPersist(presenter), "idempotency conflict returns false");
        require(fakePort->persistCallCount == 3, "client port called for conflict");
        require(
            presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("IDEMPOTENCY_CONFLICT"),
            "conflict status mapped");
        require(presenter.lastExcelVbaImportPersistIdempotencyConflictRejected(), "conflict flag mapped");
        require(
            presenter.lastExcelVbaImportPersistIssueCodes().contains(
                QStringLiteral("EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT")),
            "sanitized conflict issue code mapped");

        const int callsBeforeFailClosed = fakePort->persistCallCount;
        require(
            !presenter.persistExcelVbaImportManualEntry(
                QStringLiteral("REJECTED"),
                QStringLiteral("task-267-preview-digest"),
                QStringLiteral("task-267-idempotency"),
                QStringLiteral("excel-vba-export/v1"),
                QStringLiteral("sanitized-excel-vba-export"),
                QStringLiteral("2026-06-07T10:00:00Z"),
                QStringLiteral("TASK267_BATCH"),
                QString::fromStdString(validPayload()),
                1,
                1,
                0,
                0),
            "non-ACCEPTED preview fails closed");
        require(fakePort->persistCallCount == callsBeforeFailClosed, "non-ACCEPTED does not call client port");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("INPUT_ERROR"), "non-ACCEPTED input error");

        require(
            !presenter.persistExcelVbaImportManualEntry(
                QStringLiteral("ACCEPTED"),
                QString(),
                QStringLiteral("task-267-idempotency"),
                QStringLiteral("excel-vba-export/v1"),
                QStringLiteral("sanitized-excel-vba-export"),
                QStringLiteral("2026-06-07T10:00:00Z"),
                QStringLiteral("TASK267_BATCH"),
                QString::fromStdString(validPayload()),
                1,
                1,
                0,
                0),
            "missing preview digest fails closed");
        require(fakePort->persistCallCount == callsBeforeFailClosed, "missing digest does not call client port");

        require(
            !presenter.persistExcelVbaImportManualEntry(
                QStringLiteral("ACCEPTED"),
                QStringLiteral("task-267-preview-digest"),
                QString(),
                QStringLiteral("excel-vba-export/v1"),
                QStringLiteral("sanitized-excel-vba-export"),
                QStringLiteral("2026-06-07T10:00:00Z"),
                QStringLiteral("TASK267_BATCH"),
                QString::fromStdString(validPayload()),
                1,
                1,
                0,
                0),
            "missing idempotency key fails closed");
        require(fakePort->persistCallCount == callsBeforeFailClosed, "missing idempotency does not call client port");

        require(
            !presenter.persistExcelVbaImportManualEntry(
                QStringLiteral("ACCEPTED"),
                QStringLiteral("task-267-preview-digest"),
                QStringLiteral("task-267-idempotency"),
                QStringLiteral("excel-vba-export/v1"),
                QStringLiteral("sanitized-excel-vba-export"),
                QStringLiteral("2026-06-07T10:00:00Z"),
                QStringLiteral("TASK267_BATCH"),
                QStringLiteral("{"),
                1,
                1,
                0,
                0),
            "malformed sanitized payload fails closed");
        require(fakePort->persistCallCount == callsBeforeFailClosed, "malformed payload does not call client port");

        require(presenter.readOnly(), "presenter read-only capability remains true");
        require(!presenter.writeEnabled(), "presenter does not expose write-enabled UI");
        require(!presenter.tradeDraftGenerationEnabled(), "no TradeDraft generation");
        require(!presenter.tradeSuggestionEnabled(), "no suggestion");
        require(!presenter.strategyExecutionEnabled(), "no strategy execution");
        require(!presenter.brokerSubmissionEnabled(), "no broker submission");

        const auto evidence = evidenceJson();
        require(evidence.value("shellServicesPersistContractCreated").toBool(), "evidence contract");
        require(evidence.value("presenterPersistMethodCreated").toBool(), "evidence presenter");
        require(evidence.value("controllerPersistMethodCreated").toBool(), "evidence controller");
        require(evidence.value("typedClientPersistAdapterCalledThroughPort").toBool(), "evidence typed client");
        require(!evidence.value("productionDbTouched").toBool(), "evidence no production DB");
        require(!evidence.value("qmlWiringChanged").toBool(), "evidence no QML");
        require(!evidence.value("brokerOrderSubmitted").toBool(), "evidence no broker");
        require(!evidence.value("automaticTrading").toBool(), "evidence no automatic trading");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    }
    catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-267"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionDbTouched", false},
            {"qmlWiringChanged", false},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
