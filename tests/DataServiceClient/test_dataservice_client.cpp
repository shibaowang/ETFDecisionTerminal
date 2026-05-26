#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceClient/DataServiceClientApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

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
        / ("etfdt_dataservice_client_" + std::to_string(now));
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

QJsonObject payloadObject(const etfdt::protocol::ProtocolResponse& response)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(response.payloadJson));
    if (!document.isObject()) {
        ++gFailures;
        std::cerr << "FAILED: payload is not a JSON object: " << response.payloadJson << '\n';
        return {};
    }
    return document.object();
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    expectTrue(count.hasValue(), "count query succeeds for " + tableName);
    return count.hasValue() ? count.value() : -1;
}

void expectSuccessfulResponse(
    const etfdt::data_service_client::DataServiceClientResult<etfdt::protocol::ProtocolResponse>&
        result,
    std::string_view label)
{
    expectTrue(result.hasValue(), std::string(label) + " returns ProtocolResponse");
    if (result) {
        expectTrue(result.value().success, std::string(label) + " success=true");
    }
}

void testDataServiceClient(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "dataservice_client.db";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalDataServiceClientTest_"
        + std::to_string(QCoreApplication::applicationPid());
    auto listenResult = host.listen(socketName);
    expectTrue(listenResult.hasValue(), "ActionServiceHost listen succeeds");

    etfdt::data_service_client::DataServiceClient client;
    auto connected = client.connect(socketName, 1000);
    expectTrue(connected.hasValue(), "DataServiceClient connects");
    expectTrue(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "host accepts client");

    auto ping = client.ping();
    expectSuccessfulResponse(ping, "client.ping");
    if (ping) {
        expectTrue(payloadObject(ping.value()).value("pong").toBool(false), "ping payload pong=true");
    }

    auto health = client.health();
    expectSuccessfulResponse(health, "client.health");
    if (health) {
        expectTrue(
            payloadObject(health.value()).value("healthy").toBool(false),
            "health payload healthy=true");
    }

    auto summary = client.summary();
    expectSuccessfulResponse(summary, "client.summary");
    if (summary) {
        expectTrue(payloadObject(summary.value()).contains("activeAccount"), "summary has activeAccount");
    }

    auto accounts = client.listAccounts();
    expectSuccessfulResponse(accounts, "client.listAccounts");
    if (accounts) {
        expectTrue(
            !payloadObject(accounts.value()).value("accounts").toArray().isEmpty(),
            "listAccounts contains default account");
    }

    auto portfolios = client.listPortfolios();
    expectSuccessfulResponse(portfolios, "client.listPortfolios");
    if (portfolios) {
        expectTrue(
            !payloadObject(portfolios.value()).value("portfolios").toArray().isEmpty(),
            "listPortfolios contains default portfolio");
    }

    auto instruments = client.listInstruments();
    expectSuccessfulResponse(instruments, "client.listInstruments");
    if (instruments) {
        const auto values = payloadObject(instruments.value()).value("instruments").toArray();
        const bool hasCash = std::any_of(values.begin(), values.end(), [](const auto& value) {
            return value.toObject().value("instrumentType").toString() == "CASH";
        });
        expectTrue(hasCash, "listInstruments contains CASH");
    }

    auto strategies = client.listStrategies();
    expectSuccessfulResponse(strategies, "client.listStrategies");
    if (strategies) {
        expectTrue(payloadObject(strategies.value()).value("strategies").isArray(), "strategies is array");
    }

    auto otc = client.listOtc("not_exists");
    expectSuccessfulResponse(otc, "client.listOtc not_exists");
    if (otc) {
        expectTrue(
            payloadObject(otc.value()).value("channels").toArray().isEmpty(),
            "listOtc not_exists returns empty channels");
    }

    const int auditCountBefore = countRows(connection, "audit_log");
    auto accountingHealth = client.accountingHealth();
    expectSuccessfulResponse(accountingHealth, "client.accountingHealth");
    if (accountingHealth) {
        const auto payload = payloadObject(accountingHealth.value());
        expectEqual(payload.value("module").toString().toStdString(), "accounting", "accountingHealth module");
        expectTrue(payload.value("readOnly").toBool(false), "accountingHealth readOnly=true");
        expectTrue(
            !payload.value("replayImplemented").toBool(true),
            "accountingHealth replayImplemented=false");
        expectTrue(!payload.value("writeEnabled").toBool(true), "accountingHealth writeEnabled=false");
        const auto warnings = payload.value("warnings").toArray();
        const bool hasReplayWarning = std::any_of(warnings.begin(), warnings.end(), [](const auto& value) {
            return value.toObject().value("code").toString() == "REPLAY_NOT_IMPLEMENTED";
        });
        expectTrue(hasReplayWarning, "accountingHealth warnings include REPLAY_NOT_IMPLEMENTED");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "accountingHealth does not insert audit_log row");

    etfdt::data_service_client::AuditAppendRequest auditRequest;
    auditRequest.entityType = "SYSTEM";
    auditRequest.entityId = "1";
    auditRequest.action = "CLIENT_DEMO_AUDIT";
    auditRequest.reason = "client compatibility test";
    auditRequest.operatorName = "client-test";
    auditRequest.newValueJson = R"({"client":true})";
    auto append = client.appendAuditDemo(auditRequest);
    expectSuccessfulResponse(append, "client.appendAuditDemo");
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore + 1,
        "appendAuditDemo inserts one audit_log row");

    auto missingReasonRequest = client.makeRequest(
        "data.audit.append",
        R"({"entityType":"SYSTEM","action":"CLIENT_MISSING_REASON"})");
    auto missingReason = client.sendRaw(missingReasonRequest, 2000);
    expectTrue(missingReason.hasValue(), "missing reason returns ProtocolResponse");
    if (missingReason) {
        expectTrue(!missingReason.value().success, "missing reason success=false");
        expectEqual(
            etfdt::protocol::toString(*missingReason.value().errorCode),
            "E1002_MISSING_REQUIRED_FIELD",
            "missing reason returns E1002");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore + 1,
        "missing reason does not insert audit_log row");

    auto unknown = client.sendRaw(client.makeRequest("data.trade_log.append", R"({"reason":"blocked"})"), 2000);
    expectTrue(unknown.hasValue(), "unknown action returns ProtocolResponse");
    if (unknown) {
        expectTrue(!unknown.value().success, "unknown action success=false");
        expectEqual(
            etfdt::protocol::toString(*unknown.value().errorCode),
            "E1004_INVALID_ACTION",
            "unknown action returns E1004");
    }

    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    client.disconnect();
    expectTrue(!client.isConnected(), "client disconnects");
    auto afterDisconnect = client.accountingHealth(100);
    expectTrue(!afterDisconnect.hasValue(), "accountingHealth after disconnect returns error");
    expectTrue(
        afterDisconnect.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "accountingHealth after disconnect returns E9001");

    host.close();

    etfdt::data_service_client::DataServiceClient missingServerClient;
    auto missingServer = missingServerClient.connect(
        "ETFDecisionTerminalMissingSocket_" + std::to_string(QCoreApplication::applicationPid()),
        50);
    expectTrue(!missingServer.hasValue(), "missing socket connect fails");
    expectTrue(
        missingServer.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "missing socket connect returns E9001");

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

    testDataServiceClient(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " DataServiceClient test failure(s)" << '\n';
        return 1;
    }

    std::cout << "DataServiceClient tests passed." << '\n';
    return 0;
}
