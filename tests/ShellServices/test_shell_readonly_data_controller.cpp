#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "ShellServices/ShellServices.h"

#include <QCoreApplication>
#include <QAbstractItemModel>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QString>
#include <QThread>
#include <QVariant>

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

void expectEqual(const QString& actual, const QString& expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected.toStdString()
                  << " actual " << actual.toStdString() << '\n';
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

    etfdt::shell_services::ShellReadOnlyConnectionPresetModel presets;
    expectTrue(presets.rowCount() >= 3, "connection preset model has default presets");
    expectTrue(
        presets.roleNames().contains(etfdt::shell_services::ShellReadOnlyConnectionPresetModel::KeyRole),
        "preset model has key role");
    expectTrue(
        presets.roleNames().contains(etfdt::shell_services::ShellReadOnlyConnectionPresetModel::TitleRole),
        "preset model has title role");
    expectTrue(
        presets.roleNames().contains(etfdt::shell_services::ShellReadOnlyConnectionPresetModel::SocketNameRole),
        "preset model has socketName role");
    expectTrue(
        presets.roleNames().contains(etfdt::shell_services::ShellReadOnlyConnectionPresetModel::CommandHintRole),
        "preset model has commandHint role");
    expectTrue(
        presets.findByKey(QStringLiteral("readonly_default")) != nullptr,
        "preset model has readonly_default");
    expectTrue(
        presets.findByKey(QStringLiteral("audit_dev")) != nullptr,
        "preset model has audit_dev");
    expectTrue(
        presets.findByKey(QStringLiteral("custom")) != nullptr,
        "preset model has custom");
    const auto readonlyIndex = presets.index(0, 0);
    expectEqual(
        presets.data(readonlyIndex, etfdt::shell_services::ShellReadOnlyConnectionPresetModel::TitleRole).toString(),
        QString::fromUtf8("DataService \xE5\x8F\xAA\xE8\xAF\xBB\xE6\x9C\x8D\xE5\x8A\xA1"),
        "readonly_default title is localized");
    expectEqual(
        presets.data(readonlyIndex, Qt::DisplayRole).toString(),
        QString::fromUtf8("DataService \xE5\x8F\xAA\xE8\xAF\xBB\xE6\x9C\x8D\xE5\x8A\xA1"),
        "preset display role uses title");

    etfdt::shell_services::ShellReadOnlyTableProxyModel proxy;
    expectTrue(proxy.setSortRoleByName(QString(), true) == false, "proxy rejects missing sort role");
}

QVariant proxyData(QAbstractItemModel* model, int row, int role)
{
    return model->data(model->index(row, 0), role);
}

void testReadOnlyFilterSortModels()
{
    etfdt::shell_services::ShellReadOnlyDataController controller;

    controller.accountModel()->setRows({
        {1, "account-alpha", "Alpha account", "REAL", "MANUAL", "CNY", true, "100"},
        {2, "account-beta", "Beta account", "SIMULATED", "MANUAL", "USD", false, "200"},
    });
    expectTrue(
        controller.filteredAccountModel()->roleNames().contains(etfdt::shell_services::ShellAccountListModel::NameRole),
        "filtered account model keeps source role names");
    expectEqual(controller.filteredAccountModel()->rowCount(), 2, "filtered account default rowCount");
    controller.setAccountSearchText(QStringLiteral("Alpha"));
    expectEqual(controller.filteredAccountModel()->rowCount(), 1, "account search filters rows");
    expectEqual(
        proxyData(
            controller.filteredAccountModel(),
            0,
            etfdt::shell_services::ShellAccountListModel::NameRole)
            .toString(),
        QStringLiteral("Alpha account"),
        "account search keeps matching row");
    controller.setAccountSearchText(QString());
    expectEqual(controller.filteredAccountModel()->rowCount(), 2, "clearing account search restores rows");
    controller.setAccountActiveOnly(true);
    expectEqual(controller.filteredAccountModel()->rowCount(), 1, "account active-only filters rows");
    controller.setAccountActiveOnly(false);
    expectTrue(controller.sortAccounts(QStringLiteral("name"), false), "sortAccounts by name succeeds");
    expectEqual(
        proxyData(
            controller.filteredAccountModel(),
            0,
            etfdt::shell_services::ShellAccountListModel::NameRole)
            .toString(),
        QStringLiteral("Beta account"),
        "sortAccounts descending by name");

    controller.portfolioModel()->setRows({
        {1, "portfolio-alpha", "Alpha portfolio", "100", true},
        {2, "portfolio-beta", "Beta portfolio", "200", false},
    });
    expectEqual(controller.filteredPortfolioModel()->rowCount(), 2, "filtered portfolio default rowCount");
    controller.setPortfolioSearchText(QStringLiteral("Beta"));
    expectEqual(controller.filteredPortfolioModel()->rowCount(), 1, "portfolio search filters rows");
    controller.setPortfolioSearchText(QString());
    controller.setPortfolioActiveOnly(true);
    expectEqual(controller.filteredPortfolioModel()->rowCount(), 1, "portfolio active-only filters rows");
    controller.setPortfolioActiveOnly(false);

    controller.instrumentModel()->setRows({
        {1, "instrument-cash", "CASH", "Cash", "CASH", "CASH", "CNY", true},
        {2, "instrument-aaa", "AAA", "Disabled ETF", "ETF", "CN_A", "CNY", false},
    });
    expectEqual(controller.filteredInstrumentModel()->rowCount(), 2, "filtered instrument default rowCount");
    controller.setInstrumentSearchText(QStringLiteral("CASH"));
    expectEqual(controller.filteredInstrumentModel()->rowCount(), 1, "instrument CASH search filters rows");
    expectEqual(
        proxyData(
            controller.filteredInstrumentModel(),
            0,
            etfdt::shell_services::ShellInstrumentListModel::CodeRole)
            .toString(),
        QStringLiteral("CASH"),
        "instrument search returns CASH");
    controller.setInstrumentSearchText(QString());
    controller.setInstrumentEnabledOnly(true);
    expectEqual(controller.filteredInstrumentModel()->rowCount(), 1, "instrument enabled-only filters rows");
    expectTrue(controller.sortInstruments(QStringLiteral("code"), true), "sortInstruments by code succeeds");

    expectEqual(controller.filteredStrategyModel()->rowCount(), 0, "empty strategy proxy starts empty");
    controller.setStrategySearchText(QStringLiteral("not_exists"));
    expectEqual(controller.filteredStrategyModel()->rowCount(), 0, "strategy search on empty list does not crash");
    expectTrue(controller.sortStrategies(QStringLiteral("code"), true), "sortStrategies on empty list succeeds");

    expectEqual(controller.filteredOtcChannelModel()->rowCount(), 0, "empty otc proxy starts empty");
    controller.setOtcSearchText(QStringLiteral("not_exists"));
    expectEqual(controller.filteredOtcChannelModel()->rowCount(), 0, "otc search on empty list does not crash");
    expectTrue(controller.sortOtcChannels(QStringLiteral("priority"), true), "sortOtcChannels on empty list succeeds");

    controller.clearReadonlyFilters();
    expectEqual(controller.filteredAccountModel()->rowCount(), 2, "clearReadonlyFilters restores accounts");
    expectEqual(controller.filteredInstrumentModel()->rowCount(), 2, "clearReadonlyFilters restores instruments");
}

void testController(const std::filesystem::path& migrationPath)
{
    etfdt::shell_services::ShellReadOnlyDataController missingSocketController;
    expectEqual(missingSocketController.refreshState(), QStringLiteral("IDLE"), "initial refreshState is IDLE");
    expectTrue(!missingSocketController.isBusy(), "initial controller is not busy");
    expectTrue(!missingSocketController.canRefresh(), "initial controller cannot refresh before connect");
    expectTrue(
        missingSocketController.connectionPresetModel()->rowCount() >= 3,
        "controller exposes preset model");
    expectEqual(
        missingSocketController.selectedPresetKey(),
        QStringLiteral("readonly_default"),
        "default preset is readonly_default");
    expectEqual(
        missingSocketController.selectedSocketName(),
        QStringLiteral("ETFDataServiceReadonly"),
        "default preset socketName");
    expectTrue(
        missingSocketController.commandHint().contains(QStringLiteral("--serve-readonly")),
        "default commandHint uses serve-readonly");
    expectTrue(missingSocketController.selectPreset(QStringLiteral("audit_dev")), "audit_dev preset can be selected");
    expectEqual(
        missingSocketController.selectedSocketName(),
        QStringLiteral("ETFDataServiceAuditDev"),
        "audit_dev preset socketName");
    expectTrue(missingSocketController.selectPreset(QStringLiteral("custom")), "custom preset can be selected");
    expectTrue(missingSocketController.selectedSocketName().isEmpty(), "custom preset starts with empty socketName");
    expectTrue(!missingSocketController.connectToDataService(), "empty selected socketName connect fails");
    expectTrue(!missingSocketController.lastError().empty(), "empty selected socketName sets error");
    missingSocketController.setCustomSocketName(QStringLiteral("CustomSocket"));
    expectEqual(
        missingSocketController.selectedSocketName(),
        QStringLiteral("CustomSocket"),
        "custom socketName is runtime only");
    expectEqual(
        missingSocketController.selectedPresetKey(),
        QStringLiteral("custom"),
        "custom socketName selects custom preset");
    auto missingConnect = missingSocketController.connectToDataService(
        "ETFDecisionTerminalMissingShellDataControllerSocket_"
            + std::to_string(QCoreApplication::applicationPid()),
        50);
    expectTrue(!missingConnect.hasValue(), "missing socket connect fails");
    expectEqual(missingSocketController.refreshState(), QStringLiteral("FAILED"), "missing socket sets FAILED");
    expectTrue(!missingSocketController.lastError().empty(), "missing socket lastError");
    expectTrue(
        missingSocketController.errorStateMap().value("hasError").toBool(),
        "missing socket sets error state");
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
    const std::string socketName = "ETFDataServiceReadonly";
    auto listened = host.listen(socketName);
    expectTrue(listened.hasValue(), "readonly ActionServiceHost listen succeeds");

    const int auditLogBefore = countRows(connection, "audit_log");

    etfdt::shell_services::ShellReadOnlyDataController controller;
    expectEqual(
        controller.selectedPresetKey(),
        QStringLiteral("readonly_default"),
        "e2e controller uses readonly_default preset");
    expectTrue(controller.connectToDataService(), "controller connects");
    expectTrue(controller.connectionObject()->connected(), "connectionObject connected=true");
    expectEqual(controller.refreshState(), QStringLiteral("SUCCESS"), "connect success sets SUCCESS state");
    expectTrue(controller.canRefresh(), "controller can refresh after connect");
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

    bool nestedRefreshRejected = false;
    bool nestedRefreshAttempted = false;
    QObject::connect(
        &controller,
        &etfdt::shell_services::ShellReadOnlyDataController::stateChanged,
        [&]() {
            if (!nestedRefreshAttempted && controller.refreshState() == QStringLiteral("REFRESHING")) {
                nestedRefreshAttempted = true;
                nestedRefreshRejected = !controller.refreshAll(2000).hasValue();
            }
        });

    auto all = controller.refreshAll(2000);
    expectTrue(all.hasValue(), "refreshAll succeeds");
    expectEqual(controller.refreshState(), QStringLiteral("SUCCESS"), "refreshAll success sets SUCCESS");
    expectTrue(nestedRefreshAttempted, "busy duplicate refresh was attempted");
    expectTrue(nestedRefreshRejected, "busy duplicate refresh is rejected");
    expectEqual(controller.refreshAttemptCount(), 1, "refresh attempt count increments");
    expectEqual(controller.refreshSuccessCount(), 1, "refresh success count increments");
    expectTrue(!controller.lastSuccessAtText().isEmpty(), "last success time is populated");
    expectTrue(controller.summaryViewModel().accountCount >= 1, "refreshAll accountCount");
    expectTrue(controller.summaryViewModel().portfolioCount >= 1, "refreshAll portfolioCount");
    expectTrue(controller.summaryViewModel().instrumentCount >= 1, "refreshAll instrumentCount");
    expectTrue(controller.summaryViewModel().strategyCount >= 0, "refreshAll strategyCount");
    const int accountRowsAfterSuccess = controller.accountModel()->rowCount();
    const int failureCountBeforeThrottle = controller.refreshFailureCount();
    auto throttled = controller.refreshAll(2000);
    expectTrue(!throttled.hasValue(), "immediate refreshAll is throttled");
    expectEqual(controller.refreshThrottleCount(), 1, "refresh throttle count increments");
    expectEqual(controller.refreshFailureCount(), failureCountBeforeThrottle, "throttled refresh is not a failure");
    expectTrue(controller.accountModel()->rowCount() == accountRowsAfterSuccess, "throttled refresh preserves model rows");
    controller.setMinimumRefreshIntervalMs(0);
    expectTrue(controller.refreshAll(), "QML refreshAll wrapper succeeds");
    expectEqual(controller.refreshSuccessCount(), 2, "QML refreshAll wrapper increments success count");
    auto accountPortfolioRefresh = controller.refreshAccountsAndPortfolios(2000);
    expectTrue(accountPortfolioRefresh.hasValue(), "refreshAccountsAndPortfolios succeeds");
    expectTrue(controller.accountModel()->rowCount() >= 1, "account portfolio refresh keeps account rows");
    expectTrue(controller.portfolioModel()->rowCount() >= 1, "account portfolio refresh keeps portfolio rows");
    bool hasDefaultAccount = false;
    for (int row = 0; row < controller.accountModel()->rowCount(); ++row) {
        const auto index = controller.accountModel()->index(row, 0);
        hasDefaultAccount = hasDefaultAccount
            || controller.accountModel()
                   ->data(index, etfdt::shell_services::ShellAccountListModel::UidRole)
                   .toString()
                == QStringLiteral("00000000-0000-4000-8000-000000000101");
    }
    expectTrue(hasDefaultAccount, "account model contains default account");
    bool hasDefaultPortfolio = false;
    for (int row = 0; row < controller.portfolioModel()->rowCount(); ++row) {
        const auto index = controller.portfolioModel()->index(row, 0);
        hasDefaultPortfolio = hasDefaultPortfolio
            || controller.portfolioModel()
                   ->data(index, etfdt::shell_services::ShellPortfolioListModel::UidRole)
                   .toString()
                == QStringLiteral("00000000-0000-4000-8000-000000000201");
    }
    expectTrue(hasDefaultPortfolio, "portfolio model contains default portfolio");
    auto instrumentStrategyRefresh = controller.refreshInstrumentsAndStrategies(2000);
    expectTrue(instrumentStrategyRefresh.hasValue(), "refreshInstrumentsAndStrategies succeeds");
    expectTrue(controller.instrumentModel()->rowCount() >= 1, "instrument strategy refresh keeps instrument rows");
    expectTrue(controller.strategyModel()->rowCount() >= 0, "instrument strategy refresh keeps strategy rows");
    const auto otcRoles = controller.otcChannelModel()->roleNames();
    expectTrue(
        otcRoles.contains(etfdt::shell_services::ShellOtcChannelListModel::StrategyCodeRole),
        "otc model has strategyCode role");
    expectTrue(
        otcRoles.contains(etfdt::shell_services::ShellOtcChannelListModel::ActualCodeRole),
        "otc model has actualCode role");
    expectTrue(
        otcRoles.contains(etfdt::shell_services::ShellOtcChannelListModel::FundClassRole),
        "otc model has fundClass role");
    expectTrue(
        otcRoles.contains(etfdt::shell_services::ShellOtcChannelListModel::EnabledRole),
        "otc model has enabled role");
    controller.setSelectedStrategyCode(QString());
    auto emptyStrategyOtc = controller.refreshOtcChannels(2000);
    expectTrue(!emptyStrategyOtc.hasValue(), "refreshOtcChannels empty strategyCode fails locally");
    expectEqual(
        controller.errorStateMap().value("errorCode").toString(),
        QStringLiteral("E1002_MISSING_REQUIRED_FIELD"),
        "refreshOtcChannels empty strategyCode error code");
    controller.setSelectedStrategyCode(QStringLiteral("not_exists"));
    expectEqual(controller.selectedStrategyCode(), QStringLiteral("not_exists"), "selectedStrategyCode is set");
    auto missingOtcChannels = controller.refreshOtcChannels(2000);
    expectTrue(missingOtcChannels.hasValue(), "refreshOtcChannels missing strategy succeeds with empty result");
    expectEqual(controller.otcChannelModel()->rowCount(), 0, "missing strategy has no otc channels");
    expectTrue(controller.summaryViewModelMap().value("accountCount").toInt() >= 1, "summaryViewModel QML map accountCount");
    expectEqual(
        controller.summaryViewModelMap().value("refreshState").toString(),
        QStringLiteral("SUCCESS"),
        "summaryViewModel QML map refreshState");
    expectTrue(controller.property("accountModel").value<QObject*>() != nullptr, "accountModel Q_PROPERTY exists");
    expectTrue(controller.property("portfolioModel").value<QObject*>() != nullptr, "portfolioModel Q_PROPERTY exists");
    expectTrue(controller.property("instrumentModel").value<QObject*>() != nullptr, "instrumentModel Q_PROPERTY exists");
    expectTrue(controller.property("strategyModel").value<QObject*>() != nullptr, "strategyModel Q_PROPERTY exists");
    expectTrue(controller.property("otcChannelModel").value<QObject*>() != nullptr, "otcChannelModel Q_PROPERTY exists");
    expectTrue(
        controller.property("connectionPresetModel").value<QObject*>() != nullptr,
        "connectionPresetModel Q_PROPERTY exists");
    expectTrue(controller.selectPreset(QStringLiteral("audit_dev")), "audit_dev preset selection succeeds without writes");
    expectTrue(!controller.refreshOtc(QString()), "empty strategyCode refreshOtc fails locally");
    expectTrue(controller.errorStateMap().value("hasError").toBool(), "empty strategyCode sets error state");
    expectEqual(
        controller.errorStateMap().value("errorCode").toString(),
        QStringLiteral("E1002_MISSING_REQUIRED_FIELD"),
        "empty strategyCode error code");

    expectEqual(countRows(connection, "audit_log"), auditLogBefore, "audit_log does not increase");
    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    controller.disconnect();
    expectTrue(!controller.connectionObject()->connected(), "connectionObject connected=false after disconnect");
    expectEqual(controller.refreshState(), QStringLiteral("IDLE"), "disconnect resets refreshState to IDLE");
    expectTrue(!controller.canRefresh(), "disconnect disables refresh");
    auto afterDisconnect = controller.refreshHealth(100);
    expectTrue(!afterDisconnect.hasValue(), "refreshHealth after disconnect fails");
    expectTrue(!controller.lastError().empty(), "after disconnect lastError");
    auto allAfterDisconnect = controller.refreshAll(100);
    expectTrue(!allAfterDisconnect.hasValue(), "refreshAll after disconnect fails");
    expectEqual(controller.refreshState(), QStringLiteral("FAILED"), "refreshAll after disconnect sets FAILED");
    expectTrue(controller.accountModel()->rowCount() == accountRowsAfterSuccess, "failed refresh preserves account rows");

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
    testReadOnlyFilterSortModels();
    testController(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " ShellReadOnlyDataController test failure(s)\n";
        return 1;
    }

    std::cout << "ShellReadOnlyDataController tests passed.\n";
    return 0;
}
