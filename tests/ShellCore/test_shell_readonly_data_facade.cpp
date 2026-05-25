#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "ShellServices/ShellServices.h"

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

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
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
        / ("etfdt_shell_readonly_facade_" + std::to_string(now));
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

    auto opened = connection.open(config);
    expectTrue(opened.hasValue(), "SQLiteConnection open succeeds");
    auto migrated = etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrated.hasValue(), "initial migration succeeds");
    auto health = connection.healthCheck();
    expectTrue(health.hasValue(), "healthCheck succeeds before readonly host");
}

QJsonObject payloadObject(const etfdt::shell_services::ShellDataResponse& response)
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

void expectSuccess(
    const etfdt::shell_services::ShellDataResult<etfdt::shell_services::ShellDataResponse>& result,
    std::string_view label)
{
    expectTrue(result.hasValue(), std::string(label) + " returns response");
    if (result) {
        expectTrue(result.value().success, std::string(label) + " success=true");
    }
}

void testShellReadOnlyFacade(const std::filesystem::path& migrationPath)
{
    etfdt::shell_services::ShellReadOnlyDataFacade missingSocketFacade;
    const auto missingSocket = missingSocketFacade.connect(
        "ETFDecisionTerminalMissingShellFacadeSocket_"
            + std::to_string(QCoreApplication::applicationPid()),
        50);
    expectTrue(!missingSocket.hasValue(), "connect missing socket fails");
    expectTrue(missingSocketFacade.connectionState().disconnected, "missing socket state disconnected");
    expectTrue(!missingSocketFacade.lastError().empty(), "missing socket lastError is set");

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "shell_readonly_facade.db";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalShellReadonlyFacadeTest_"
        + std::to_string(QCoreApplication::applicationPid());
    auto listened = host.listen(socketName);
    expectTrue(listened.hasValue(), "readonly ActionServiceHost listen succeeds");

    const int auditLogBefore = countRows(connection, "audit_log");

    etfdt::shell_services::ShellReadOnlyDataFacade facade;
    auto connected = facade.connect(socketName, 1000);
    expectTrue(connected.hasValue(), "ShellReadOnlyDataFacade connects");
    expectTrue(facade.isConnected(), "facade isConnected");
    expectTrue(facade.connectionState().connected, "facade state connected");
    expectTrue(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "host accepts facade client");

    auto ping = facade.ping();
    expectSuccess(ping, "facade.ping");
    if (ping) {
        expectTrue(payloadObject(ping.value()).value("pong").toBool(false), "ping payload pong=true");
    }

    auto health = facade.health();
    expectSuccess(health, "facade.health");
    if (health) {
        expectTrue(payloadObject(health.value()).value("healthy").toBool(false), "health payload healthy=true");
    }

    auto summary = facade.summary();
    expectSuccess(summary, "facade.summary");
    if (summary) {
        expectTrue(payloadObject(summary.value()).contains("activeAccount"), "summary has activeAccount");
    }

    auto accounts = facade.listAccounts();
    expectSuccess(accounts, "facade.listAccounts");
    if (accounts) {
        expectTrue(!payloadObject(accounts.value()).value("accounts").toArray().isEmpty(), "accounts has default row");
    }

    auto portfolios = facade.listPortfolios();
    expectSuccess(portfolios, "facade.listPortfolios");
    if (portfolios) {
        expectTrue(!payloadObject(portfolios.value()).value("portfolios").toArray().isEmpty(), "portfolios has default row");
    }

    auto instruments = facade.listInstruments();
    expectSuccess(instruments, "facade.listInstruments");
    if (instruments) {
        const auto values = payloadObject(instruments.value()).value("instruments").toArray();
        const bool hasCash = std::any_of(values.begin(), values.end(), [](const auto& value) {
            return value.toObject().value("instrumentType").toString() == "CASH";
        });
        expectTrue(hasCash, "instruments contains CASH");
    }

    auto strategies = facade.listStrategies();
    expectSuccess(strategies, "facade.listStrategies");
    if (strategies) {
        expectTrue(payloadObject(strategies.value()).value("strategies").isArray(), "strategies is array");
    }

    auto otc = facade.listOtc("not_exists");
    expectSuccess(otc, "facade.listOtc not_exists");
    if (otc) {
        expectTrue(payloadObject(otc.value()).value("channels").toArray().isEmpty(), "unknown strategy has no otc channels");
    }

    auto emptyOtc = facade.listOtc("   ");
    expectTrue(!emptyOtc.hasValue(), "empty strategyCode fails locally");
    expectTrue(
        emptyOtc.error().errorCode == etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        "empty strategyCode returns E1002");

    auto snapshot = facade.loadSnapshot();
    expectTrue(snapshot.hasValue(), "loadSnapshot succeeds");
    if (snapshot) {
        expectTrue(snapshot.value().healthy, "snapshot healthy=true");
        expectTrue(!snapshot.value().summaryJson.empty(), "snapshot summaryJson populated");
        expectTrue(snapshot.value().accountCount > 0, "snapshot accountCount");
        expectTrue(snapshot.value().portfolioCount > 0, "snapshot portfolioCount");
        expectTrue(snapshot.value().instrumentCount > 0, "snapshot instrumentCount");
        expectTrue(snapshot.value().strategyCount >= 0, "snapshot strategyCount");
    }

    expectEqual(countRows(connection, "audit_log"), auditLogBefore, "audit_log does not increase");
    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    facade.disconnect();
    expectTrue(!facade.isConnected(), "facade disconnects");
    auto afterDisconnect = facade.ping(100);
    expectTrue(!afterDisconnect.hasValue(), "ping after disconnect fails");
    expectTrue(
        afterDisconnect.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "ping after disconnect returns E9001");
    expectTrue(!facade.lastError().empty(), "after disconnect failure has lastError");

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

    testShellReadOnlyFacade(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " ShellReadOnlyDataFacade test failure(s)\n";
        return 1;
    }

    std::cout << "ShellReadOnlyDataFacade tests passed.\n";
    return 0;
}
