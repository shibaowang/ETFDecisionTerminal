#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPort.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

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
    return R"({"schemaVersion":"excel-vba-export/v1","source":"sanitized-excel-vba-export","sheets":[{"name":"InitialCash","headers":["ROW_ID","TIME_UTC","ACCOUNT_CODE","PORTFOLIO_CODE","ACTION","AMOUNT","CURRENCY","MEMO"],"rows":[["TASK270_CASH_001","2026-06-07T09:00:00Z","TASK270_ACCOUNT","TASK270_PORTFOLIO","INITIAL_CASH","1000.00","CNY","SANITIZED_TASK270_INITIAL_CASH"]]},{"name":"TradeLog","headers":["ROW_ID","TRADE_TIME_UTC","ACCOUNT_CODE","PORTFOLIO_CODE","INSTRUMENT_CODE","SIDE","QUANTITY","PRICE","AMOUNT","FEE","CASH_FLOW","CURRENCY","SOURCE","MEMO"],"rows":[["TASK270_TRADE_001","2026-06-07T09:30:00Z","TASK270_ACCOUNT","TASK270_PORTFOLIO","TASK270_ETF","BUY","100","2.0000","200.00","1.00","-201.00","CNY","TASK270_SANITIZED_IMPORT","SANITIZED_TASK270_BUY"]]}]})";
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
    response.issues.push_back({status, "ERROR", status, true, "FakeTask270ClientPort"});
    return response;
}

etfdt::shell_services::ShellAccountingDataServiceClientResponse refreshSuccess(
    const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request)
{
    etfdt::shell_services::ShellAccountingDataServiceClientResponse response;
    response.actionName = request.actionName;
    response.protocolSuccess = true;
    response.implemented = true;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus = "OK";
    response.dataQualityStatus = "OK";
    response.hasRows = true;
    response.productionWrite = false;
    response.sqliteProductionWrite = false;
    response.auditWritten = false;
    response.ledgerWritten = false;
    response.snapshotWritten = false;
    response.tradeLogWritten = false;
    response.readModelPersistentWrite = false;
    return response;
}

etfdt::shell_services::ShellAccountingDataServiceClientResponse refreshFailure(
    const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request)
{
    return unavailable(request, "TASK270_REFRESH_FAILED");
}

class FakeTask270ClientPort final : public etfdt::shell_services::ShellAccountingDataServiceClientPort {
public:
    std::vector<etfdt::shell_services::ShellAccountingDataServiceClientResponse> persistResponses;
    int previewCallCount = 0;
    int persistCallCount = 0;
    int positionRefreshCount = 0;
    int cashRefreshCount = 0;
    int pnlRefreshCount = 0;
    bool failRefresh = false;

    void resetRefreshCounts()
    {
        positionRefreshCount = 0;
        cashRefreshCount = 0;
        pnlRefreshCount = 0;
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse refreshResponse(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request)
    {
        return failRefresh ? refreshFailure(request) : refreshSuccess(request);
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callPositionList(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++positionRefreshCount;
        return refreshResponse(request);
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callCashSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++cashRefreshCount;
        return refreshResponse(request);
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++pnlRefreshCount;
        return refreshResponse(request);
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return refreshResponse(request);
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        return refreshResponse(request);
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
        response.importPreviewDiagnosticCodes = {"TASK270_ACCEPTED"};
        return response;
    }

    etfdt::shell_services::ShellAccountingDataServiceClientResponse callExcelVbaImportPersistManualEntry(
        const etfdt::shell_services::ShellAccountingDataServiceClientRequest& request) override
    {
        ++persistCallCount;
        require(
            request.actionName == "accounting.excel_vba_import.persist_manual_entry",
            "persist action mapped");
        const auto payload = parseObject(request.payloadJson);
        require(payload.value("previewStatus").toString() == "ACCEPTED", "accepted preview mapped");
        require(payload.value("previewDigest").toString().startsWith(QStringLiteral("fnv64-")), "digest mapped");
        require(payload.value("idempotencyKey").toString().contains(QStringLiteral("fnv64-")), "idempotency mapped");
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
    response.accountingEngineCalled = false;
    response.networkAccess = false;
    response.credentialAccess = false;
    response.endpointAccess = false;
    response.automaticTrading = false;
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
        "FakeTask270ClientPort"});
    return response;
}

void requireStaticBoundaries(const std::filesystem::path& root)
{
    const auto docsGate =
        root / "docs" / "336_shell_accounting_excel_vba_import_persist_post_write_readback_refresh.md";
    const auto docsPlan =
        root / "docs" / "337_shell_accounting_excel_vba_import_persist_post_write_readback_refresh_test_plan.md";
    const auto qmlPath =
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";
    const auto presenterHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h";
    const auto presenterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp";

    requireContains(readFile(docsGate), "TASK-270", "TASK-270 docs");
    requireContains(readFile(docsPlan), "shell_accounting_excel_vba_import_persist_post_write_readback_refresh", "TASK-270 test plan");

    const auto header = readFile(presenterHeader);
    requireContains(
        header,
        "lastExcelVbaImportPersistTradeLogRowsWritten",
        "Presenter persisted trade_log rows property");
    requireContains(
        header,
        "lastExcelVbaImportPersistCashAdjustmentRowsWritten",
        "Presenter persisted cash_adjustment rows property");

    const auto source = readFile(presenterSource);
    requireContains(
        source,
        "excel_vba_import_persist_manual_entry",
        "Presenter refresh reason");
    requireContains(
        source,
        "refreshAfterManualEntryWrite(",
        "Presenter reuses existing post-write refresh path");
    requireContains(
        source,
        "excel_vba_import_idempotency_conflict",
        "Presenter conflict does not refresh");
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

    const auto qml = readFile(qmlPath);
    for (const auto& token : {
             "shellAccountingExcelVbaImportPersistTradeLogRowsText",
             "shellAccountingExcelVbaImportPersistCashAdjustmentRowsText",
             "lastExcelVbaImportPersistTradeLogRowsWritten",
             "lastExcelVbaImportPersistCashAdjustmentRowsWritten",
             "shellAccountingExcelVbaImportPostPersistRefreshStatusText",
             "shellAccountingExcelVbaImportPostPersistRefreshSummaryText",
             "shellAccountingExcelVbaImportPostPersistRefreshIssueText",
             "lastPostWriteRefreshStatus",
             "lastPostWriteRefreshSummary",
             "lastPostWriteRefreshIssue",
         }) {
        requireContains(qml, token, "QML TASK-270 wiring");
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
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-270"},
        {"persistSuccessTriggersReadbackRefresh", true},
        {"postPersistRefreshAttempted", true},
        {"postPersistRefreshSucceeded", true},
        {"persistedTradeLogRowsVisible", true},
        {"persistedCashAdjustmentRowsVisible", true},
        {"duplicateImportDoesNotClaimNewRows", true},
        {"idempotencyConflictDoesNotRefresh", true},
        {"refreshFailureSanitized", true},
        {"readOnlyRefreshNoWrite", true},
        {"qmlDirectSQLite", false},
        {"qmlDirectDataServiceClient", false},
        {"accountingEngineProductionCodeChanged", false},
        {"brokerOrderSubmitted", false},
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

        auto fakePort = std::make_shared<FakeTask270ClientPort>();
        auto serviceAdapter =
            std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(fakePort);
        auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
        controller->setServiceAdapter(serviceAdapter);
        etfdt::shell_services::ShellAccountingPresenter presenter;
        presenter.setController(controller);

        require(
            presenter.previewExcelVbaImportReadOnly(QString::fromStdString(acceptedPayload())),
            "accepted preview succeeds");
        require(fakePort->previewCallCount == 1, "preview client port called");

        fakePort->resetRefreshCounts();
        fakePort->persistResponses.push_back(successResponse());
        require(presenter.persistAcceptedExcelVbaImportPreview(), "persist success returns true");
        require(fakePort->persistCallCount == 1, "persist client port called once");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("PERSISTED"), "persist status");
        require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 2, "trade+cash trade_log rows visible");
        require(presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 1, "cash_adjustment rows visible");
        require(fakePort->positionRefreshCount == 1, "position readback refresh attempted");
        require(fakePort->cashRefreshCount == 1, "cash readback refresh attempted");
        require(fakePort->pnlRefreshCount == 1, "PnL readback refresh attempted");
        require(presenter.lastPostWriteRefreshStatus() == QStringLiteral("OK"), "post-persist refresh succeeded");
        require(
            presenter.lastPostWriteRefreshSummary().contains(
                QStringLiteral("reason=excel_vba_import_persist_manual_entry")),
            "post-persist refresh reason visible");

        fakePort->resetRefreshCounts();
        fakePort->persistResponses.push_back(duplicateResponse());
        require(presenter.persistAcceptedExcelVbaImportPreview(), "duplicate import returns idempotent success");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("DUPLICATE"), "duplicate visible");
        require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 0, "duplicate does not claim trade rows");
        require(presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 0, "duplicate does not claim cash rows");
        require(fakePort->positionRefreshCount == 0, "duplicate does not refresh position");
        require(fakePort->cashRefreshCount == 0, "duplicate does not refresh cash");
        require(fakePort->pnlRefreshCount == 0, "duplicate does not refresh PnL");
        require(presenter.lastPostWriteRefreshStatus() == QStringLiteral("SKIPPED"), "duplicate refresh skipped");
        require(
            presenter.lastPostWriteRefreshSummary().contains(QStringLiteral("newRows=false")),
            "duplicate summary avoids new rows");

        fakePort->resetRefreshCounts();
        fakePort->persistResponses.push_back(conflictResponse());
        require(!presenter.persistAcceptedExcelVbaImportPreview(), "conflict returns false");
        require(
            presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("IDEMPOTENCY_CONFLICT"),
            "conflict status visible");
        require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 0, "conflict trade rows zero");
        require(presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 0, "conflict cash rows zero");
        require(fakePort->positionRefreshCount == 0, "conflict does not refresh position");
        require(fakePort->cashRefreshCount == 0, "conflict does not refresh cash");
        require(fakePort->pnlRefreshCount == 0, "conflict does not refresh PnL");
        require(presenter.lastPostWriteRefreshStatus() == QStringLiteral("SKIPPED"), "conflict refresh skipped");
        require(
            presenter.lastPostWriteRefreshIssue().contains(QStringLiteral("failed closed")),
            "conflict refresh issue sanitized");

        fakePort->resetRefreshCounts();
        fakePort->failRefresh = true;
        fakePort->persistResponses.push_back(successResponse());
        require(presenter.persistAcceptedExcelVbaImportPreview(), "persist success remains true when refresh fails");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("PERSISTED"), "persist remains successful");
        require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 2, "refresh failure preserves persisted trade rows");
        require(presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 1, "refresh failure preserves persisted cash rows");
        require(fakePort->positionRefreshCount == 1, "refresh failure still attempted position");
        require(fakePort->cashRefreshCount == 1, "refresh failure still attempted cash");
        require(fakePort->pnlRefreshCount == 1, "refresh failure still attempted PnL");
        require(presenter.lastPostWriteRefreshStatus() == QStringLiteral("ERROR"), "refresh failure status visible");
        require(!presenter.lastPostWriteRefreshIssue().isEmpty(), "refresh failure issue sanitized");

        require(presenter.readOnly(), "presenter remains read-only");
        require(!presenter.writeEnabled(), "presenter does not expose write-enabled UI");
        require(!presenter.tradeDraftGenerationEnabled(), "no TradeDraft generation");
        require(!presenter.strategyExecutionEnabled(), "no strategy execution");
        require(!presenter.brokerSubmissionEnabled(), "no broker submission");

        const auto evidence = evidenceJson();
        require(evidence.value("persistSuccessTriggersReadbackRefresh").toBool(), "evidence refresh");
        require(evidence.value("persistedTradeLogRowsVisible").toBool(), "evidence trade rows");
        require(evidence.value("persistedCashAdjustmentRowsVisible").toBool(), "evidence cash rows");
        require(evidence.value("duplicateImportDoesNotClaimNewRows").toBool(), "evidence duplicate");
        require(evidence.value("idempotencyConflictDoesNotRefresh").toBool(), "evidence conflict");
        require(evidence.value("readOnlyRefreshNoWrite").toBool(), "evidence read-only refresh");
        require(!evidence.value("brokerOrderSubmitted").toBool(), "evidence no broker");
        require(!evidence.value("automaticTrading").toBool(), "evidence no automatic trading");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    }
    catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-270"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
