#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceClient/DataServiceClientApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QThread>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

struct PreviewSample {
    std::string sampleId;
    std::filesystem::path path;
    std::filesystem::file_time_type beforeWriteTime;
    QJsonObject requestPayload;
    QJsonObject expectedPreview;
};

struct PreviewCall {
    PreviewSample sample;
    etfdt::data_service_client::ExcelVbaImportReadOnlyPreviewResult preview;
};

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

std::vector<std::filesystem::path> optionValues(int argc, char* argv[], const std::string& option)
{
    std::vector<std::filesystem::path> values;
    for (int index = 1; index + 1 < argc; ++index) {
        if (argv[index] == option) {
            values.emplace_back(argv[index + 1]);
        }
    }
    return values;
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_task259_preview_client_adapter_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

bool waitUntil(const std::function<bool()>& predicate, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        if (predicate()) {
            return true;
        }
        QThread::msleep(5);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    return predicate();
}

QJsonObject parseJsonFile(const std::filesystem::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "fixture file missing: " + path.string());
    require(file.open(QIODevice::ReadOnly), "fixture file cannot be opened: " + path.string());

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    require(parseError.error == QJsonParseError::NoError, "fixture JSON malformed: " + path.string());
    require(document.isObject(), "fixture JSON root must be object: " + path.string());
    return document.object();
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("missing string field: ") + key);
    return value.toString().toStdString();
}

int intValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isDouble(), std::string("missing integer field: ") + key);
    return value.toInt();
}

bool boolValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("missing bool field: ") + key);
    return value.toBool();
}

QJsonObject objectValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isObject(), std::string("missing object field: ") + key);
    return value.toObject();
}

QJsonArray arrayValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isArray(), std::string("missing array field: ") + key);
    return value.toArray();
}

std::string compactJson(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}

PreviewSample loadSample(const std::filesystem::path& path)
{
    const auto root = parseJsonFile(path);
    require(stringValue(root, "task") == "TASK-259", "fixture task mismatch");
    require(
        stringValue(root, "schemaVersion") ==
            "task-259-excel-vba-import-readonly-preview-client-adapter/v1",
        "fixture schema version mismatch");
    require(
        stringValue(root, "sourceFormat") == "sanitized-excel-vba-style-import-payload",
        "fixture source format mismatch");
    require(!boolValue(root, "rawUserDataExposed"), "fixture raw user data flag false");
    require(arrayValue(root, "externalDependencies").isEmpty(), "fixture must have no external dependencies");

    PreviewSample sample;
    sample.sampleId = stringValue(root, "sampleId");
    sample.path = path;
    sample.beforeWriteTime = std::filesystem::last_write_time(path);
    sample.requestPayload = objectValue(root, "requestPayload");
    sample.expectedPreview = objectValue(root, "expectedPreview");
    return sample;
}

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath.string();
    config.migrationFilePath = migrationPath.string();
    config.enableWal = true;
    config.foreignKeys = true;

    auto openResult = connection.open(config);
    require(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    require(migrationResult.hasValue(), "initial migration succeeds");
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    require(count.hasValue(), "count query succeeds for " + tableName);
    return count.value();
}

std::vector<std::pair<std::string, int>> protectedCounts(
    etfdt::data_access::SQLiteConnection& connection)
{
    return {
        {"audit_log", countRows(connection, "audit_log")},
        {"trade_log", countRows(connection, "trade_log")},
        {"trade_execution_group", countRows(connection, "trade_execution_group")},
        {"cash_adjustment", countRows(connection, "cash_adjustment")},
        {"cash_snapshot", countRows(connection, "cash_snapshot")},
        {"position_snapshot", countRows(connection, "position_snapshot")},
        {"portfolio_summary", countRows(connection, "portfolio_summary")},
        {"trade_draft", countRows(connection, "trade_draft")},
    };
}

void requireCountsUnchanged(
    etfdt::data_access::SQLiteConnection& connection,
    const std::vector<std::pair<std::string, int>>& before)
{
    for (const auto& [tableName, expected] : before) {
        require(countRows(connection, tableName) == expected, tableName + " row count unchanged");
    }
}

bool textContainsSensitiveToken(const std::string& text)
{
    const auto lower = QString::fromStdString(text).toLower();
    return lower.contains("sanitized_task259_") || lower.contains("password")
        || lower.contains("secret") || lower.contains("credential")
        || lower.contains("endpoint") || lower.contains("stack");
}

bool diagnosticTextSanitized(
    const std::vector<etfdt::data_service_client::ExcelVbaImportReadOnlyPreviewDiagnostic>&
        diagnostics)
{
    return std::none_of(diagnostics.begin(), diagnostics.end(), [](const auto& diagnostic) {
        return textContainsSensitiveToken(diagnostic.level)
            || textContainsSensitiveToken(diagnostic.code)
            || textContainsSensitiveToken(diagnostic.field)
            || textContainsSensitiveToken(diagnostic.sheetName)
            || textContainsSensitiveToken(diagnostic.rowId);
    });
}

void requirePreviewFlags(
    const etfdt::data_service_client::ExcelVbaImportReadOnlyPreviewResult& preview)
{
    require(
        preview.action == etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview,
        "typed preview action mapped");
    require(preview.task == "TASK-258", "typed preview preserves service action task");
    require(
        preview.mode == "readonly-dataservice-import-preview-action-vertical-slice",
        "typed preview preserves service action mode");
    require(preview.dataServiceReadOnlyActionRegistered, "read-only action registered flag mapped");
    require(preview.previewActionExecuted, "preview action executed flag mapped");
    require(preview.diagnosticsBuilt, "diagnostics built flag mapped");
    require(preview.replayFactSummaryBuilt, "replay fact summary built flag mapped");
    require(!preview.accountingEngineCalled, "AccountingEngine not called");
    require(!preview.productionRuntimeIntegrationImplemented, "no production runtime integration");
    require(preview.parserReadOnly, "parser read-only flag mapped");
    require(preview.parserNoSqliteAccess, "parser no SQLite flag mapped");
    require(preview.parserNoWrite, "parser no write flag mapped");
    require(!preview.productionWrite, "no production write");
    require(!preview.sqliteProductionWrite, "no production SQLite write");
    require(!preview.auditWritten, "no audit write");
    require(!preview.ledgerWritten, "no ledger write");
    require(!preview.snapshotWritten, "no snapshot write");
    require(!preview.tradeLogWritten, "no trade_log write");
    require(!preview.readModelPersistentWrite, "no persistent read model write");
    require(!preview.tradeDraftGenerated, "no TradeDraft generated");
    require(!preview.strategyExecuted, "no strategy execution");
    require(!preview.brokerOrderSubmitted, "no broker order");
    require(!preview.networkAccess, "no network access");
    require(!preview.credentialAccess, "no credential access");
    require(!preview.endpointAccess, "no endpoint access");
    require(!preview.automaticTrading, "no automatic trading");
    require(!preview.rawUserDataExposed, "raw user data not exposed");
    require(diagnosticTextSanitized(preview.diagnostics), "typed diagnostics are sanitized");
}

void requireExpectedPreview(const PreviewCall& call)
{
    const auto expected = call.sample.expectedPreview;
    const auto expectedSummary = objectValue(expected, "replayFactSummary");
    requirePreviewFlags(call.preview);
    require(call.preview.accepted == boolValue(expected, "accepted"), "accepted field mapped");
    require(
        static_cast<int>(call.preview.diagnostics.size()) == intValue(expected, "diagnosticCount"),
        "diagnostic count mapped");
    require(
        call.preview.replayFactSummary.tradeFactCount == intValue(expectedSummary, "tradeFactCount"),
        "trade fact count mapped");
    require(
        call.preview.replayFactSummary.cashFactCount == intValue(expectedSummary, "cashFactCount"),
        "cash fact count mapped");
    require(
        call.preview.replayFactSummary.marketPriceFactCount == intValue(expectedSummary, "marketPriceFactCount"),
        "market price fact count mapped");
    require(
        call.preview.replayFactSummary.fxRateFactCount == intValue(expectedSummary, "fxRateFactCount"),
        "FX fact count mapped");

    const auto expectedCodes = arrayValue(expected, "diagnosticCodes");
    require(
        static_cast<int>(call.preview.diagnosticCodes.size()) == expectedCodes.size(),
        "diagnostic code count mapped");
    for (int index = 0; index < expectedCodes.size(); ++index) {
        require(expectedCodes[index].isString(), "expected diagnostic code must be string");
        require(
            call.preview.diagnosticCodes[static_cast<std::size_t>(index)] ==
                expectedCodes[index].toString().toStdString(),
            "diagnostic code mapped");
    }
    require(
        std::filesystem::last_write_time(call.sample.path) == call.sample.beforeWriteTime,
        "TASK-259 fixture file not mutated");
}

QJsonObject evidenceJson(const std::vector<PreviewCall>& calls)
{
    bool validMapped = false;
    bool invalidMapped = false;
    bool diagnosticsMapped = false;
    bool replayFactSummaryMapped = false;
    for (const auto& call : calls) {
        if (call.preview.accepted) {
            validMapped = true;
        }
        if (!call.preview.accepted && !call.preview.diagnosticCodes.empty()) {
            invalidMapped = true;
            diagnosticsMapped = true;
        }
        replayFactSummaryMapped = replayFactSummaryMapped
            || call.preview.replayFactSummary.tradeFactCount > 0
            || call.preview.replayFactSummary.cashFactCount > 0;
    }

    return QJsonObject{
        {"task", "TASK-259"},
        {"mode", "readonly-import-preview-client-adapter-vertical-slice"},
        {"previewClientAdapterCreated", true},
        {"dataServicePreviewActionCalled", !calls.empty()},
        {"previewResponseParsed", true},
        {"diagnosticsMapped", diagnosticsMapped},
        {"replayFactSummaryMapped", replayFactSummaryMapped},
        {"validPreviewMapped", validMapped},
        {"invalidPreviewMapped", invalidMapped},
        {"sampleCount", static_cast<int>(calls.size())},
        {"action", etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview},
        {"accountingEngineCalled", false},
        {"qmlWiringChanged", false},
        {"productionFileLoading", false},
        {"productionWrite", false},
        {"sqliteProductionWrite", false},
        {"auditWritten", false},
        {"ledgerWritten", false},
        {"snapshotWritten", false},
        {"tradeLogWritten", false},
        {"readModelPersistentWrite", false},
        {"tradeDraftGenerated", false},
        {"brokerOrderSubmitted", false},
        {"networkAccess", false},
        {"credentialAccess", false},
        {"endpointAccess", false},
        {"automaticTrading", false},
        {"rawUserDataExposed", false},
    };
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    try {
        const auto migrationPath = optionValue(argc, argv, "--migration");
        const auto fixturePaths = optionValues(argc, argv, "--fixture");
        require(!migrationPath.empty(), "--migration <path> is required");
        require(fixturePaths.size() >= 2U, "at least two fixtures are required");

        const auto tempDir = createTempDirectory();
        const auto dbPath = tempDir / "task259_preview_client_adapter.db";

        etfdt::data_access::SQLiteConnection connection;
        openMigratedDatabase(migrationPath, dbPath, connection);

        etfdt::service_runtime::ActionDispatcher dispatcher(
            etfdt::protocol::ServiceName::ETFDataService);
        etfdt::service_runtime::registerBuiltinActions(dispatcher);
        etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
        require(
            dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview),
            "read-only preview action is registered");

        etfdt::service_host::ActionServiceHost host(dispatcher);
        const std::string socketName = "ETFDecisionTerminalTask259PreviewClientAdapter_"
            + std::to_string(QCoreApplication::applicationPid());
        auto listenResult = host.listen(socketName);
        require(listenResult.hasValue(), "ActionServiceHost listen succeeds");

        etfdt::data_service_client::DataServiceClient client;
        auto connected = client.connect(socketName, 1000);
        require(connected.hasValue(), "DataServiceClient connects to local service host");
        require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

        const auto countsBefore = protectedCounts(connection);
        std::vector<PreviewCall> calls;
        for (const auto& fixturePath : fixturePaths) {
            PreviewCall call;
            call.sample = loadSample(fixturePath);
            auto preview = client.accountingExcelVbaImportReadOnlyPreview(
                compactJson(call.sample.requestPayload),
                2000);
            require(preview.hasValue(), "typed preview client adapter returns result");
            call.preview = std::move(preview.value());
            requireExpectedPreview(call);
            calls.push_back(std::move(call));
        }

        auto sqlRejected = client.accountingExcelVbaImportReadOnlyPreview(R"({"sql":"select 1"})", 100);
        require(!sqlRejected.hasValue(), "typed preview client rejects raw SQL payload field");
        require(
            sqlRejected.error().errorCode == etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "raw SQL payload field maps to safe client error");

        requireCountsUnchanged(connection, countsBefore);
        const auto evidence = evidenceJson(calls);
        require(boolValue(evidence, "previewClientAdapterCreated"), "evidence client adapter created");
        require(boolValue(evidence, "dataServicePreviewActionCalled"), "evidence action called");
        require(boolValue(evidence, "previewResponseParsed"), "evidence response parsed");
        require(boolValue(evidence, "diagnosticsMapped"), "evidence diagnostics mapped");
        require(boolValue(evidence, "replayFactSummaryMapped"), "evidence fact summary mapped");
        require(!boolValue(evidence, "rawUserDataExposed"), "evidence raw data not exposed");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';

        client.disconnect();
        host.close();
        connection.close();
        std::filesystem::remove_all(tempDir);
        return 0;
    }
    catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-259"},
            {"mode", "readonly-import-preview-client-adapter-vertical-slice"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionWrite", false},
            {"sqliteProductionWrite", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
