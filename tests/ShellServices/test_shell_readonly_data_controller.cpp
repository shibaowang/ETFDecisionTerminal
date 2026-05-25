#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "ShellServices/ShellServices.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QString>
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
        / ("etfdt_shell_readonly_controller_" + std::to_string(now));
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
    expectTrue(health.hasValue(), "healthCheck succeeds before controller test");
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    expectTrue(count.hasValue(), "count query succeeds for " + tableName);
    return count.hasValue() ? count.value() : -1;
}

void testModelRoles()
{
    etfdt::shell_services::ShellAccountListModel accounts;
    expectTrue(
        accounts.roleNames().contains(etfdt::shell_services::ShellAccountListModel::NameRole),
        "account model has name role");
    expectTrue(
        accounts.roleNames().contains(etfdt::shell_services::ShellAccountListModel::InitialCashTextRole),
        "account model has initialCashText role");

    etfdt::shell_services::ShellPortfolioListModel portfolios;
    expectTrue(
        portfolios.roleNames().contains(etfdt::shell_services::ShellPortfolioListModel::BasePositionRatioTextRole),
        "portfolio model has basePositionRatioText role");

    etfdt::shell_services::ShellInstrumentListModel instruments;
    expectTrue(
        instruments.roleNames().contains(etfdt::shell_services::ShellInstrumentListModel::InstrumentTypeRole),
        "instrument model has instrumentType role");

    etfdt::shell_services::ShellStrategyListModel strategies;
    expectTrue(
        strategies.roleNames().contains(etfdt::shell_services::ShellStrategyListModel::StrategyCodeRole),
        "strategy model has strategyCode role");
}

void testController(const std::filesystem::path& migrationPath)
{
    etfdt::shell_services::ShellReadOnlyDataController missingSocketController;
    auto missingConnect = missingSocketController.connectToDataService(
        "ETFDecisionTerminalMissingShellDataControllerSocket_"
            + std::to_string(QCoreApplication::applicationPid()),
        50);
    expectTrue(!missingConnect.hasValue(), "missing socket connect fails");
    expectTrue(!missingSocketController.lastError().empty(), "missing socket lastError");
    expectTrue(
        !missingSocketController.connectToDataService(QStringLiteral("ETFDecisionTerminalMissingShellDataControllerSocket_Qml")),
        "QML connect wrapper reports missing socket failure");

    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "shell_readonly_controller.db";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalShellDataControllerTest_"
        + std::to_string(QCoreApplication::applicationPid());
    auto listened = host.listen(socketName);
    expectTrue(listened.hasValue(), "readonly ActionServiceHost listen succeeds");

    const int auditLogBefore = countRows(connection, "audit_log");

    etfdt::shell_services::ShellReadOnlyDataController controller;
    auto connected = controller.connectToDataService(socketName, 1000);
    expectTrue(connected.hasValue(), "controller connects");
    expectTrue(controller.connectionObject()->connected(), "connectionObject connected=true");
    expectTrue(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "host accepts controller client");

    auto health = controller.refreshHealth(2000);
    expectTrue(health.hasValue(), "refreshHealth succeeds");
    expectTrue(controller.connectionObject()->healthy(), "connectionObject healthy=true");
    expectTrue(controller.summaryViewModel().healthy, "summary healthy=true");

    auto summary = controller.refreshSummary(2000);
    expectTrue(summary.hasValue(), "refreshSummary succeeds");
    expectTrue(!controller.summaryViewModel().summaryText.empty(), "summary text populated");

    auto accounts = controller.refreshAccounts(2000);
    expectTrue(accounts.hasValue(), "refreshAccounts succeeds");
    expectTrue(controller.accountModel()->rowCount() >= 1, "account model has rows");
    const auto firstAccount = controller.accountModel()->index(0, 0);
    expectTrue(
        !controller.accountModel()
             ->data(firstAccount, etfdt::shell_services::ShellAccountListModel::NameRole)
             .toString()
             .isEmpty(),
        "account name populated");

    auto portfolios = controller.refreshPortfolios(2000);
    expectTrue(portfolios.hasValue(), "refreshPortfolios succeeds");
    expectTrue(controller.portfolioModel()->rowCount() >= 1, "portfolio model has rows");

    auto instruments = controller.refreshInstruments(2000);
    expectTrue(instruments.hasValue(), "refreshInstruments succeeds");
    expectTrue(controller.instrumentModel()->rowCount() >= 1, "instrument model has rows");
    bool hasCash = false;
    for (int row = 0; row < controller.instrumentModel()->rowCount(); ++row) {
        const auto index = controller.instrumentModel()->index(row, 0);
        hasCash = hasCash
            || controller.instrumentModel()
                   ->data(index, etfdt::shell_services::ShellInstrumentListModel::InstrumentTypeRole)
                   .toString()
                == QStringLiteral("CASH");
    }
    expectTrue(hasCash, "instrument model contains CASH");

    auto strategies = controller.refreshStrategies(2000);
    expectTrue(strategies.hasValue(), "refreshStrategies succeeds");
    expectTrue(controller.strategyModel()->rowCount() >= 0, "strategy model refreshes");

    auto all = controller.refreshAll(2000);
    expectTrue(all.hasValue(), "refreshAll succeeds");
    expectTrue(controller.summaryViewModel().accountCount >= 1, "refreshAll accountCount");
    expectTrue(controller.summaryViewModel().portfolioCount >= 1, "refreshAll portfolioCount");
    expectTrue(controller.summaryViewModel().instrumentCount >= 1, "refreshAll instrumentCount");
    expectTrue(controller.summaryViewModel().strategyCount >= 0, "refreshAll strategyCount");
    expectTrue(controller.refreshAll(), "QML refreshAll wrapper succeeds");
    expectTrue(controller.summaryViewModelMap().value("accountCount").toInt() >= 1, "summaryViewModel QML map accountCount");
    expectTrue(controller.property("accountModel").value<QObject*>() != nullptr, "accountModel Q_PROPERTY exists");
    expectTrue(controller.property("portfolioModel").value<QObject*>() != nullptr, "portfolioModel Q_PROPERTY exists");
    expectTrue(controller.property("instrumentModel").value<QObject*>() != nullptr, "instrumentModel Q_PROPERTY exists");
    expectTrue(controller.property("strategyModel").value<QObject*>() != nullptr, "strategyModel Q_PROPERTY exists");

    expectEqual(countRows(connection, "audit_log"), auditLogBefore, "audit_log does not increase");
    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    controller.disconnect();
    expectTrue(!controller.connectionObject()->connected(), "connectionObject connected=false after disconnect");
    auto afterDisconnect = controller.refreshHealth(100);
    expectTrue(!afterDisconnect.hasValue(), "refreshHealth after disconnect fails");
    expectTrue(!controller.lastError().empty(), "after disconnect lastError");

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

    testModelRoles();
    testController(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " ShellReadOnlyDataController test failure(s)\n";
        return 1;
    }

    std::cout << "ShellReadOnlyDataController tests passed.\n";
    return 0;
}
