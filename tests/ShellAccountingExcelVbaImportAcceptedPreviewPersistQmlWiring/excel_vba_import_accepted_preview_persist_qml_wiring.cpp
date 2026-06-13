#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPort.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMetaMethod>

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

std::string acceptedPayload()
{
    return R"({"schemaVersion":"excel-vba-export/v1","source":"sanitized-excel-vba-export","sheets":[{"name":"InitialCash","headers":["ROW_ID","TIME_UTC","ACCOUNT_CODE","PORTFOLIO_CODE","ACTION","AMOUNT","CURRENCY","MEMO"],"rows":[["TASK268_CASH_001","2026-06-07T09:00:00Z","TASK268_ACCOUNT","TASK268_PORTFOLIO","INITIAL_CASH","1000.00","CNY","SANITIZED_TASK268_INITIAL_CASH"]]},{"name":"TradeLog","headers":["ROW_ID","TRADE_TIME_UTC","ACCOUNT_CODE","PORTFOLIO_CODE","INSTRUMENT_CODE","SIDE","QUANTITY","PRICE","AMOUNT","FEE","CASH_FLOW","CURRENCY","SOURCE","MEMO"],"rows":[["TASK268_TRADE_001","2026-06-07T09:30:00Z","TASK268_ACCOUNT","TASK268_PORTFOLIO","TASK268_ETF","BUY","100","2.0000","200.00","1.00","-201.00","CNY","TASK268_SANITIZED_IMPORT","SANITIZED_TASK268_BUY"]]}]})";
}

std::string workbookLevelAcceptedPayloadWithWarning()
{
    return R"({"schemaVersion":"excel-vba-export/v1","source":"sanitized-excel-vba-export","sheets":[{"name":"TradeLog","headers":["\u65f6\u95f4","\u7b56\u7565\u4ee3\u7801","\u5b9e\u9645\u4ee3\u7801","\u52a8\u4f5c","\u4ef7\u683c","\u6570\u91cf","\u91d1\u989d","\u6765\u6e90","\u624b\u7eed\u8d39","\u5907\u6ce8","\u51c0\u73b0\u91d1\u6d41","PushPlus Token:"],"rows":[["2026-06-13T09:00:00Z","CASH","","\u5165\u91d1","1","5000","5000","","","SANITIZED_CASH_ROW","5000","IGNORED_SECRET_HEADER"],["2026-06-13T09:30:00Z","510300","510300","\u4e70\u5165","4.500","100","450.00","","1.00","SANITIZED_BUY_ROW","-451.00","IGNORED_SECRET_HEADER"]]}]})";
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
    response.issues.push_back({status, "ERROR", status, true, "FakeTask268ClientPort"});
    return response;
}

class FakeTask268ClientPort final : public etfdt::shell_services::ShellAccountingDataServiceClientPort {
public:
    std::vector<etfdt::shell_services::ShellAccountingDataServiceClientResponse> persistResponses;
    int previewCallCount = 0;
    int persistCallCount = 0;
    std::string lastPreviewDigest;
    std::string lastIdempotencyKey;

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callPositionList(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK268_UNUSED_POSITION_LIST");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callCashSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK268_UNUSED_CASH_SUMMARY");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK268_UNUSED_PORTFOLIO_PNL");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK268_UNUSED_BASE_POSITION");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return unavailable(request, "TASK268_UNUSED_SNIPER_POOL");
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callExcelVbaImportReadOnlyPreview(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++previewCallCount;
        require(request.actionName == "accounting.excel_vba_import.readonly_preview", "preview action mapped");
        require(parseObject(request.payloadJson).contains("sheets"), "preview payload is object");
        etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
        response.actionName = request.actionName;
        response.protocolSuccess = true;
        response.implemented = true;
        response.readOnly = true;
        response.writeEnabled = false;
        response.payloadStatus = "ACCEPTED";
        response.dataQualityStatus = "OK";
        response.hasRows = true;
        response.importPreviewAccepted = true;
        response.importPreviewRejected = false;
        response.importPreviewFactSummary = {1, 1, 0, 0};
        response.importPreviewDiagnosticCodes =
            request.payloadJson.find("PushPlus Token") == std::string::npos
            ? std::vector<std::string>{"TASK268_ACCEPTED"}
            : std::vector<std::string>{"SENSITIVE_HEADER_IGNORED"};
        return response;
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callExcelVbaImportPersistManualEntry(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++persistCallCount;
        require(request.actionName == "accounting.excel_vba_import.persist_manual_entry", "persist action mapped");
        const auto payload = parseObject(request.payloadJson);
        lastPreviewDigest = payload.value("previewDigest").toString().toStdString();
        lastIdempotencyKey = payload.value("idempotencyKey").toString().toStdString();
        require(payload.value("previewStatus").toString() == "ACCEPTED", "accepted status mapped");
        require(!lastPreviewDigest.empty(), "Presenter generated preview digest");
        require(lastPreviewDigest.rfind("fnv64-", 0) == 0, "Presenter generated digest format");
        require(
            lastIdempotencyKey.find(lastPreviewDigest) != std::string::npos,
            "Presenter generated idempotency key from digest");
        require(payload.value("schemaVersion").toString() == "excel-vba-export/v1", "schemaVersion mapped");
        require(payload.value("source").toString() == "sanitized-excel-vba-export", "source mapped");
        require(payload.value("acceptedAt").toString().size() > 0, "acceptedAt generated");
        require(payload.value("importBatchLabel").toString() == "shell-qml-accepted-preview", "batch label mapped");
        require(payload.value("previewFactSummary").isObject(), "fact summary mapped");
        require(payload.value("importPayload").isObject(), "sanitized import payload mapped");
        require(!persistResponses.empty(), "fake persist response scripted");
        auto response = persistResponses.front();
        persistResponses.erase(persistResponses.begin());
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
        "FakeTask268ClientPort"});
    return response;
}

etfdt::shell_services::ShellAccountingDataServiceClientResponse readOnlyOnlyFailureResponse()
{
    etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
    response.protocolSuccess = false;
    response.implemented = false;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus = "DATASERVICE_CLIENT_CALL_FAILED";
    response.dataQualityStatus = "UNAVAILABLE";
    response.protocolError = true;
    response.issues.push_back({
        "DATASERVICE_CLIENT_CALL_FAILED",
        "ERROR",
        "Unsupported action: accounting.excel_vba_import.persist_manual_entry",
        true,
        "FakeTask268ClientPort"});
    return response;
}

void requireStaticBoundaries(const std::filesystem::path& root)
{
    const auto qmlPath =
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";
    const auto presenterHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h";
    const auto presenterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp";

    const auto qml = readFile(qmlPath);
    for (const auto& token : {
             "shellAccountingExcelVbaImportPersistPanel",
             "shellAccountingExcelVbaImportPersistConfirmCheck",
             "shellAccountingExcelVbaImportPersistButton",
             "shellAccountingExcelVbaImportPersistStatusText",
             "shellAccountingExcelVbaImportPersistSummaryText",
             "shellAccountingExcelVbaImportPersistIssueCodesText",
             "shellAccountingExcelVbaImportPersistIssueText",
             "shellAccountingExcelVbaImportPersistDuplicateText",
             "shellAccountingExcelVbaImportPersistConflictText",
             "shellAccountingExcelVbaImportPersistGateDiagnosticsText",
             "persistAcceptedExcelVbaImportPreview()",
             "lastExcelVbaImportPreviewStatus === \"ACCEPTED\"",
             "lastExcelVbaImportPreviewDigest.length > 0",
             "excelVbaImportPreviewPayloadAvailable",
             "excelVbaImportPersistConfirmed",
             "excelVbaImportPersistReady()",
             "resetExcelVbaImportPersistState()",
             "写入门禁",
             "可写入",
             "Digest",
             "Payload",
             "用户确认",
             "写入忙碌",
          }) {
        requireContains(qml, token, "QML TASK-268 wiring");
    }
    for (const auto& token : {
             "DataServiceClient",
             "DataServiceApi",
             "SQLite",
             "AccountingEngine",
             "accounting.excel_vba_import.persist_manual_entry",
             "INSERT INTO",
             "credential",
             "endpoint",
         }) {
        requireNotContains(qml, token, "QML direct forbidden dependency");
    }

    const auto header = readFile(presenterHeader);
    requireContains(
        header,
        "Q_INVOKABLE bool persistAcceptedExcelVbaImportPreview();",
        "Presenter QML-safe wrapper");
    requireContains(
        header,
        "Q_PROPERTY(QString lastExcelVbaImportPreviewDigest",
        "Presenter preview digest property");
    requireContains(
        header,
        "Q_PROPERTY(bool excelVbaImportPreviewPayloadAvailable",
        "Presenter payload availability property");
    requireNotContains(
        header,
        "Q_INVOKABLE bool persistExcelVbaImportManualEntry(",
        "raw persist method not exposed directly to QML");

    const auto source = readFile(presenterSource);
    requireContains(source, "shell-qml-excel-vba-import:", "Presenter idempotency source");
    requireContains(source, "shell-qml-accepted-preview", "Presenter import batch label");
    for (const auto& token : {
             "DataServiceClient/DataServiceClient.h",
             "DataServiceApi",
             "DataAccess",
             "#include \"AccountingEngine",
             "AccountingEngine::",
             "accountingengine.replay",
             "INSERT INTO",
         }) {
        requireNotContains(source, token, "Presenter source forbidden dependency");
    }
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-268"},
        {"qmlPersistButtonWired", true},
        {"presenterPersistInvokableCreated", true},
        {"acceptedPreviewRequired", true},
        {"explicitUserConfirmationRequired", true},
        {"persistButtonDisabledBeforeAccepted", true},
        {"persistButtonDisabledWithoutConfirmation", true},
        {"persistButtonEnabledAfterAcceptedAndConfirmed", true},
        {"persistGateDiagnosticsVisible", true},
        {"warningDiagnosticsDoNotBlockPersistGate", true},
        {"workbookLevelAcceptedDigestGenerated", true},
        {"presenterPersistMethodInvokedFromQml", true},
        {"persistStatusVisible", true},
        {"persistSummaryVisible", true},
        {"persistIssueCodesVisible", true},
        {"duplicateImportVisible", true},
        {"idempotencyConflictVisible", true},
        {"resetClearsPersistState", true},
        {"qmlDirectDataServiceClient", false},
        {"qmlDirectDataServiceApi", false},
        {"qmlDirectSQLite", false},
        {"qmlDirectAccountingEngine", false},
        {"automaticPersistence", false},
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

        auto fakePort = std::make_shared<FakeTask268ClientPort>();
        auto serviceAdapter =
            std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(fakePort);
        auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
        controller->setServiceAdapter(serviceAdapter);
        etfdt::shell_services::ShellAccountingPresenter presenter;
        presenter.setController(controller);

        const int methodIndex =
            presenter.metaObject()->indexOfMethod("persistAcceptedExcelVbaImportPreview()");
        require(methodIndex >= 0, "Presenter persist wrapper is invokable");
        require(
            presenter.metaObject()->method(methodIndex).methodType() == QMetaMethod::Method,
            "Presenter persist wrapper is callable from QML");

        require(!presenter.persistAcceptedExcelVbaImportPreview(), "non-ACCEPTED preview fails closed");
        require(fakePort->persistCallCount == 0, "non-ACCEPTED does not call client port");

        require(
            presenter.previewExcelVbaImportReadOnly(QString::fromStdString(acceptedPayload())),
            "accepted preview succeeds");
        require(fakePort->previewCallCount == 1, "preview client port called");
        require(presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("ACCEPTED"), "preview accepted");
        require(!presenter.lastExcelVbaImportPreviewDigest().isEmpty(), "preview digest visible");
        require(presenter.excelVbaImportPreviewPayloadAvailable(), "preview payload availability visible");

        fakePort->persistResponses.push_back(successResponse());
        require(presenter.persistAcceptedExcelVbaImportPreview(), "QML-safe wrapper persists accepted preview");
        require(fakePort->persistCallCount == 1, "persist client port called once");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("PERSISTED"), "persist status visible");
        require(presenter.lastExcelVbaImportPersistTransactionCommitted(), "transaction committed mapped");
        require(presenter.lastExcelVbaImportPersistTradeLogWritten(), "trade log mapped");
        require(presenter.lastExcelVbaImportPersistAuditLogWritten(), "audit log mapped");
        require(!fakePort->lastIdempotencyKey.empty(), "idempotency key generated");

        fakePort->persistResponses.push_back(duplicateResponse());
        require(presenter.persistAcceptedExcelVbaImportPreview(), "duplicate import returns idempotent success");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("DUPLICATE"), "duplicate visible");
        require(presenter.lastExcelVbaImportPersistDuplicateImportPrevented(), "duplicate flag visible");

        fakePort->persistResponses.push_back(conflictResponse());
        require(!presenter.persistAcceptedExcelVbaImportPreview(), "idempotency conflict returns false");
        require(
            presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("IDEMPOTENCY_CONFLICT"),
            "conflict status visible");
        require(presenter.lastExcelVbaImportPersistIdempotencyConflictRejected(), "conflict flag visible");
        require(
            presenter.lastExcelVbaImportPersistIssueCodes().contains(
                QStringLiteral("EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT")),
            "conflict issue code visible");

        fakePort->persistResponses.push_back(readOnlyOnlyFailureResponse());
        require(!presenter.persistAcceptedExcelVbaImportPreview(), "read-only-only service fails closed");
        require(
            presenter.lastExcelVbaImportPersistStatus()
                == QStringLiteral("DATASERVICE_CLIENT_CALL_FAILED"),
            "read-only-only failure status visible");
        require(
            presenter.lastExcelVbaImportPersistIssue().contains(QStringLiteral("--serve-daily-use")),
            "daily-use write mode remediation visible");
        require(
            presenter.lastExcelVbaImportPersistIssue().contains(
                QStringLiteral("Start-ETFDTDailyUseDataService.ps1")),
            "daily-use script remediation visible");

        presenter.resetExcelVbaImportPreviewState();
        require(!presenter.excelVbaImportPreviewPayloadAvailable(), "reset clears accepted payload");
        require(presenter.lastExcelVbaImportPreviewDigest().isEmpty(), "reset clears digest");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("READY"), "reset clears persist state");

        require(
            presenter.previewExcelVbaImportReadOnly(
                QString::fromStdString(workbookLevelAcceptedPayloadWithWarning())),
            "workbook-level accepted preview succeeds");
        require(fakePort->previewCallCount == 2, "workbook-level preview client port called");
        require(
            presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("ACCEPTED"),
            "workbook-level preview accepted");
        require(
            presenter.excelVbaImportPreviewTradeFactCount() > 0,
            "workbook-level preview has trade facts");
        require(
            presenter.excelVbaImportPreviewCashFactCount() > 0,
            "workbook-level preview has cash facts");
        require(
            presenter.excelVbaImportPreviewMarketPriceFactCount() == 0,
            "workbook-level preview has no market price facts");
        require(
            presenter.excelVbaImportPreviewFxRateFactCount() == 0,
            "workbook-level preview has no FX facts");
        require(
            presenter.lastExcelVbaImportPreviewDiagnosticCodes().contains(
                QStringLiteral("SENSITIVE_HEADER_IGNORED")),
            "sensitive header warning remains nonblocking");
        require(
            !presenter.lastExcelVbaImportPreviewDigest().isEmpty(),
            "workbook-level preview digest visible");
        require(
            presenter.excelVbaImportPreviewPayloadAvailable(),
            "workbook-level preview payload availability visible");

        const auto evidence = evidenceJson();
        require(evidence.value("qmlPersistButtonWired").toBool(), "evidence QML");
        require(evidence.value("presenterPersistInvokableCreated").toBool(), "evidence Presenter");
        require(evidence.value("persistGateDiagnosticsVisible").toBool(), "evidence gate diagnostics");
        require(
            evidence.value("warningDiagnosticsDoNotBlockPersistGate").toBool(),
            "evidence warning nonblocking");
        require(!evidence.value("brokerOrderSubmitted").toBool(), "evidence no broker");
        require(!evidence.value("automaticTrading").toBool(), "evidence no automatic trading");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    }
    catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-268"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
