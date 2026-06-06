#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
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
    etfdt::protocol::ProtocolResponse response;
    QJsonObject payload;
};

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
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

QJsonObject parseJsonFile(const std::filesystem::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "fixture file missing");
    require(file.open(QIODevice::ReadOnly), "fixture file cannot be opened");

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    require(parseError.error == QJsonParseError::NoError, "fixture JSON malformed");
    require(document.isObject(), "fixture JSON root must be object");
    return document.object();
}

QJsonObject parsePayloadObject(const std::string& payloadJson)
{
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payloadJson), &parseError);
    require(parseError.error == QJsonParseError::NoError, "response payload JSON malformed");
    require(document.isObject(), "response payload must be object");
    return document.object();
}

std::string compactJson(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}

PreviewSample loadSample(const std::filesystem::path& path)
{
    const auto root = parseJsonFile(path);
    require(stringValue(root, "task") == "TASK-258", "fixture task mismatch");
    require(
        stringValue(root, "schemaVersion") ==
            "task-258-excel-vba-import-readonly-dataservice-preview-action/v1",
        "fixture schema version mismatch");
    require(
        stringValue(root, "sourceFormat") == "sanitized-excel-vba-style-import-payload",
        "fixture source format mismatch");
    require(arrayValue(root, "externalDependencies").isEmpty(), "fixture must have no external dependencies");

    PreviewSample sample;
    sample.sampleId = stringValue(root, "sampleId");
    sample.path = path;
    sample.beforeWriteTime = std::filesystem::last_write_time(path);
    sample.requestPayload = objectValue(root, "requestPayload");
    sample.expectedPreview = objectValue(root, "expectedPreview");
    return sample;
}

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task258-dataservice-preview-msg";
    request.traceId = "task258-dataservice-preview-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview;
    request.timestampUtc = "2026-06-06T00:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_task258_dataservice_preview_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath.string();
    auto openResult = connection.open(config);
    require(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migration = etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    require(migration.hasValue(), "initial migration succeeds");
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

bool sanitizedPayload(const std::string& payloadJson)
{
    const auto text = QString::fromStdString(payloadJson);
    return !text.contains("SANITIZED_TASK258_INITIAL_CASH", Qt::CaseInsensitive) &&
           !text.contains("SANITIZED_TASK258_BUY", Qt::CaseInsensitive) &&
           !text.contains("payload", Qt::CaseInsensitive) &&
           !text.contains("password", Qt::CaseInsensitive) &&
           !text.contains("secret", Qt::CaseInsensitive) &&
           !text.contains("stack", Qt::CaseInsensitive);
}

PreviewCall invokePreview(
    etfdt::service_runtime::ActionDispatcher& dispatcher,
    PreviewSample sample)
{
    PreviewCall call;
    call.sample = std::move(sample);
    call.response = dispatcher.dispatch(envelope(compactJson(call.sample.requestPayload)));
    require(call.response.success, "preview action response success");
    call.payload = parsePayloadObject(call.response.payloadJson);
    return call;
}

void assertPreviewCall(const PreviewCall& call)
{
    const auto expected = call.sample.expectedPreview;
    require(stringValue(call.payload, "task") == "TASK-258", "preview task field");
    require(
        stringValue(call.payload, "mode") ==
            "readonly-dataservice-import-preview-action-vertical-slice",
        "preview mode field");
    require(
        stringValue(call.payload, "action") ==
            etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview,
        "preview action field");
    require(boolValue(call.payload, "dataServiceReadOnlyActionRegistered"), "read-only action registered flag");
    require(boolValue(call.payload, "previewActionExecuted"), "preview action executed flag");
    require(boolValue(call.payload, "parserBoundaryCalled"), "parser boundary called flag");
    require(boolValue(call.payload, "diagnosticsBuilt"), "diagnostics built flag");
    require(boolValue(call.payload, "replayFactSummaryBuilt"), "replay fact summary built flag");
    require(!boolValue(call.payload, "accountingEngineCalled"), "AccountingEngine not called");
    require(!boolValue(call.payload, "productionRuntimeIntegrationImplemented"), "no production runtime integration");
    require(!boolValue(call.payload, "productionWrite"), "no production write");
    require(!boolValue(call.payload, "sqliteProductionWrite"), "no production SQLite write");
    require(!boolValue(call.payload, "auditWritten"), "no audit write");
    require(!boolValue(call.payload, "ledgerWritten"), "no ledger write");
    require(!boolValue(call.payload, "snapshotWritten"), "no snapshot write");
    require(!boolValue(call.payload, "tradeLogWritten"), "no trade_log write");
    require(!boolValue(call.payload, "readModelPersistentWrite"), "no persistent read model write");
    require(!boolValue(call.payload, "tradeDraftGenerated"), "no TradeDraft generated");
    require(!boolValue(call.payload, "strategyExecuted"), "no strategy execution");
    require(!boolValue(call.payload, "brokerOrderSubmitted"), "no broker order");
    require(!boolValue(call.payload, "networkAccess"), "no network access");
    require(!boolValue(call.payload, "credentialAccess"), "no credential access");
    require(!boolValue(call.payload, "endpointAccess"), "no endpoint access");
    require(!boolValue(call.payload, "automaticTrading"), "no automatic trading");
    require(!boolValue(call.payload, "rawUserDataExposed"), "raw user data not exposed");
    require(sanitizedPayload(call.response.payloadJson), "preview response is sanitized");

    require(boolValue(call.payload, "accepted") == boolValue(expected, "accepted"), "accepted parity");
    require(arrayValue(call.payload, "diagnostics").size() == intValue(expected, "diagnosticCount"), "diagnostic count");

    const auto summary = objectValue(call.payload, "replayFactSummary");
    const auto expectedSummary = objectValue(expected, "replayFactSummary");
    require(intValue(summary, "tradeFactCount") == intValue(expectedSummary, "tradeFactCount"), "trade fact count");
    require(intValue(summary, "cashFactCount") == intValue(expectedSummary, "cashFactCount"), "cash fact count");
    require(
        intValue(summary, "marketPriceFactCount") == intValue(expectedSummary, "marketPriceFactCount"),
        "market price fact count");
    require(intValue(summary, "fxRateFactCount") == intValue(expectedSummary, "fxRateFactCount"), "fx fact count");

    require(
        std::filesystem::last_write_time(call.sample.path) == call.sample.beforeWriteTime,
        "fixture file not mutated");
}

QJsonObject evidenceJson(const std::vector<PreviewCall>& calls)
{
    bool validAccepted = false;
    bool invalidRejected = false;
    bool sanitized = true;
    for (const auto& call : calls) {
        const bool expectedAccepted = boolValue(call.sample.expectedPreview, "accepted");
        if (expectedAccepted && boolValue(call.payload, "accepted")) {
            validAccepted = true;
        }
        if (!expectedAccepted && !boolValue(call.payload, "accepted") && !arrayValue(call.payload, "diagnostics").isEmpty()) {
            invalidRejected = true;
        }
        sanitized = sanitized && sanitizedPayload(call.response.payloadJson);
    }

    return QJsonObject{
        {"task", "TASK-258"},
        {"mode", "readonly-dataservice-import-preview-action-vertical-slice"},
        {"dataServiceReadOnlyActionRegistered", true},
        {"parserBoundaryCalled", true},
        {"previewActionExecuted", true},
        {"diagnosticsBuilt", true},
        {"replayFactSummaryBuilt", true},
        {"accountingEngineCalled", false},
        {"productionRuntimeIntegrationImplemented", false},
        {"validPreviewAccepted", validAccepted},
        {"invalidPreviewRejected", invalidRejected},
        {"sampleCount", static_cast<int>(calls.size())},
        {"productionWrite", false},
        {"sqliteProductionWrite", false},
        {"auditWritten", false},
        {"ledgerWritten", false},
        {"snapshotWritten", false},
        {"tradeLogWritten", false},
        {"readModelPersistentWrite", false},
        {"tradeDraftGenerated", false},
        {"strategyExecuted", false},
        {"brokerOrderSubmitted", false},
        {"networkAccess", false},
        {"credentialAccess", false},
        {"endpointAccess", false},
        {"automaticTrading", false},
        {"rawUserDataExposed", !sanitized},
    };
}

}  // namespace

int main(int argc, char* argv[])
{
    try {
        require(argc >= 4, "usage: test <migration> <fixture>...");
        const std::filesystem::path migrationPath = argv[1];
        const auto tempDir = createTempDirectory();
        const auto dbPath = tempDir / "task258_preview.db";

        etfdt::data_access::SQLiteConnection connection;
        openMigratedDatabase(migrationPath, dbPath, connection);

        etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
        etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
        require(
            dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview),
            "read-only preview action is registered");

        const auto countsBefore = protectedCounts(connection);
        std::vector<PreviewCall> calls;
        for (int index = 2; index < argc; ++index) {
            auto sample = loadSample(argv[index]);
            auto call = invokePreview(dispatcher, std::move(sample));
            assertPreviewCall(call);
            calls.push_back(std::move(call));
        }
        requireCountsUnchanged(connection, countsBefore);

        const auto evidence = evidenceJson(calls);
        require(boolValue(evidence, "validPreviewAccepted"), "valid preview accepted");
        require(boolValue(evidence, "invalidPreviewRejected"), "invalid preview rejected");
        require(!boolValue(evidence, "rawUserDataExposed"), "raw user data not exposed in evidence");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';

        connection.close();
        std::filesystem::remove_all(tempDir);
        return 0;
    }
    catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-258"},
            {"mode", "readonly-dataservice-import-preview-action-vertical-slice"},
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
