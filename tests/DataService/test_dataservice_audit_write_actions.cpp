#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "Protocol/Protocol.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "Transport/Transport.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_dataservice_audit_actions_" + std::to_string(now));
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

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson = "{}")
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "dataservice-audit-msg-" + action;
    request.traceId = "dataservice-audit-trace-" + action;
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-05-24T16:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

QJsonObject parseJsonObject(const std::string& json)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (!document.isObject()) {
        ++gFailures;
        std::cerr << "FAILED: response is not a JSON object: " << json << '\n';
        return {};
    }
    return document.object();
}

bool sendAndWait(
    etfdt::transport::LocalSocketClient& client,
    const std::string& json,
    std::vector<std::string>& responses)
{
    const auto before = responses.size();
    auto sent = client.send(json);
    expectTrue(sent.hasValue(), "client sends request frame");
    if (!sent) {
        return false;
    }
    return waitUntil([&]() { return responses.size() > before; }, 2000);
}

QJsonObject sendEnvelope(
    etfdt::transport::LocalSocketClient& client,
    const etfdt::protocol::MessageEnvelope& request,
    std::vector<std::string>& responses,
    std::string_view label)
{
    const bool received = sendAndWait(client, etfdt::protocol::toJsonString(request), responses);
    expectTrue(received, std::string(label) + " response received");
    if (!received) {
        return {};
    }
    return parseJsonObject(responses.back());
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    expectTrue(count.hasValue(), "count query succeeds for " + tableName);
    return count.hasValue() ? count.value() : -1;
}

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;
    config.enableWal = true;
    config.foreignKeys = true;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");
    auto health = connection.healthCheck();
    expectTrue(health.hasValue(), "healthCheck succeeds before serving");
}

void connectClient(
    etfdt::service_host::ActionServiceHost& host,
    etfdt::transport::LocalSocketClient& client,
    const std::string& socketName)
{
    auto listenResult = host.listen(socketName);
    expectTrue(listenResult.hasValue(), "ActionServiceHost listen succeeds");
    auto connected = client.connectToServer(socketName, 1000);
    expectTrue(connected.hasValue(), "LocalSocketClient connects");
    expectTrue(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "host accepts client");
}

void testDevAuditActions(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "audit_actions.db";
    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    expectTrue(
        etfdt::data_service_api::isWriteActionAllowed(
            etfdt::data_service_api::kActionDataAuditAppend),
        "data.audit.append is in write whitelist");
    expectTrue(
        !etfdt::data_service_api::isWriteActionAllowed("data.trade_log.append"),
        "data.trade_log.append is not in write whitelist");

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    etfdt::transport::LocalSocketClient client;
    std::vector<std::string> responses;
    client.setMessageCallback([&](const std::string& json) {
        responses.push_back(json);
    });

    const std::string socketName = "ETFDecisionTerminalAuditActionTest_"
        + std::to_string(QCoreApplication::applicationPid());
    connectClient(host, client, socketName);

    auto ping = sendEnvelope(
        client,
        envelope(etfdt::service_runtime::kActionSystemPing),
        responses,
        "system.ping");
    expectTrue(ping.value("success").toBool(false), "system.ping success=true");

    const int auditCountBefore = countRows(connection, "audit_log");
    const std::string validPayload =
        R"({"entityType":"SYSTEM","entityId":"1","action":"DEMO_AUDIT","reason":"manual audit test","operatorName":"unit-test","oldValue":{},"newValue":{"changed":true}})";
    auto append = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataAuditAppend, validPayload),
        responses,
        "data.audit.append");
    expectTrue(append.value("success").toBool(false), "data.audit.append success=true");
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore + 1,
        "data.audit.append inserts one audit_log row");

    etfdt::data_access::AuditLogRepository auditLogs(connection);
    auto recent = auditLogs.listRecentAuditLogs(1);
    expectTrue(recent.hasValue(), "listRecentAuditLogs succeeds after action");
    if (recent && !recent.value().empty()) {
        const auto& record = recent.value().front();
        expectEqual(record.entityType, "SYSTEM", "audit_log entity_type recorded");
        expectEqual(record.action, "DEMO_AUDIT", "audit_log action recorded");
        expectEqual(record.reason, "manual audit test", "audit_log reason recorded");
    }

    const int auditCountAfterSuccess = countRows(connection, "audit_log");
    auto missingReason = sendEnvelope(
        client,
        envelope(
            etfdt::data_service_api::kActionDataAuditAppend,
            R"({"entityType":"SYSTEM","action":"MISSING_REASON"})"),
        responses,
        "data.audit.append missing reason");
    expectTrue(!missingReason.value("success").toBool(true), "missing reason success=false");
    expectEqual(
        missingReason.value("errorCode").toString().toStdString(),
        "E1002_MISSING_REQUIRED_FIELD",
        "missing reason returns E1002");
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountAfterSuccess,
        "missing reason does not insert audit_log row");

    auto missingEntityType = sendEnvelope(
        client,
        envelope(
            etfdt::data_service_api::kActionDataAuditAppend,
            R"({"action":"MISSING_ENTITY","reason":"missing entity type"})"),
        responses,
        "data.audit.append missing entityType");
    expectTrue(!missingEntityType.value("success").toBool(true), "missing entityType success=false");
    expectEqual(
        missingEntityType.value("errorCode").toString().toStdString(),
        "E1002_MISSING_REQUIRED_FIELD",
        "missing entityType returns E1002");

    const bool invalidReceived = sendAndWait(client, "{ invalid }", responses);
    expectTrue(invalidReceived, "invalid JSON response received");
    if (invalidReceived) {
        auto invalid = parseJsonObject(responses.back());
        expectTrue(!invalid.value("success").toBool(true), "invalid JSON success=false");
        const auto code = invalid.value("errorCode").toString().toStdString();
        expectTrue(
            code == "E1001_INVALID_JSON" || code == "E1002_MISSING_REQUIRED_FIELD",
            "invalid JSON returns protocol error");
    }

    auto unknownWrite = sendEnvelope(
        client,
        envelope("data.trade_log.append", R"({"sql":"INSERT"})"),
        responses,
        "unknown write action");
    expectTrue(!unknownWrite.value("success").toBool(true), "unknown write action success=false");
    expectEqual(
        unknownWrite.value("errorCode").toString().toStdString(),
        "E1004_INVALID_ACTION",
        "unknown write action returns E1004");

    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    client.disconnect();
    expectTrue(waitUntil([&]() { return !client.isConnected(); }, 1000), "client disconnects");
    host.close();
    connection.close();
    std::filesystem::remove_all(tempDir);
}

void testReadOnlyRejectsAuditAppend(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "readonly_rejects_audit.db";
    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    etfdt::transport::LocalSocketClient client;
    std::vector<std::string> responses;
    client.setMessageCallback([&](const std::string& json) {
        responses.push_back(json);
    });

    const std::string socketName = "ETFDecisionTerminalReadOnlyAuditRejectTest_"
        + std::to_string(QCoreApplication::applicationPid());
    connectClient(host, client, socketName);

    auto response = sendEnvelope(
        client,
        envelope(
            etfdt::data_service_api::kActionDataAuditAppend,
            R"({"entityType":"SYSTEM","action":"SHOULD_NOT_WRITE","reason":"readonly"})"),
        responses,
        "--serve-readonly rejects data.audit.append");
    expectTrue(!response.value("success").toBool(true), "readonly audit append success=false");
    expectEqual(
        response.value("errorCode").toString().toStdString(),
        "E1004_INVALID_ACTION",
        "readonly host rejects data.audit.append with E1004");
    expectEqual(countRows(connection, "audit_log"), 0, "readonly host does not write audit_log");

    client.disconnect();
    expectTrue(waitUntil([&]() { return !client.isConnected(); }, 1000), "readonly client disconnects");
    host.close();
    connection.close();
    std::filesystem::remove_all(tempDir);
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto migrationPath = optionValue(argc, argv, "--migration");
    if (migrationPath.empty()) {
        std::cerr << "--migration <path> is required\n";
        return 1;
    }

    testDevAuditActions(migrationPath);
    testReadOnlyRejectsAuditAppend(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " DataService audit action test failure(s)" << '\n';
        return 1;
    }

    std::cout << "DataService audit write action tests passed." << '\n';
    return 0;
}
