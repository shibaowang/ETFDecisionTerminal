#include "DataAccess/DataAccess.h"
#include "Watchdog/Watchdog.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QThread>

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
        / ("etfdt_watchdog_" + std::to_string(now));
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
        QThread::msleep(20);
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

etfdt::watchdog::ServiceProcessConfig dataServiceConfig(
    const std::filesystem::path& dataServiceExe,
    const std::filesystem::path& dbPath,
    const std::string& socketName)
{
    etfdt::watchdog::ServiceProcessConfig config;
    config.serviceName = "ETFDataService";
    config.executablePath = dataServiceExe.string();
    config.arguments = {"--serve-readonly", "--db", dbPath.string(), "--socket-name", socketName};
    config.socketName = socketName;
    config.workingDirectory = std::filesystem::current_path().string();
    config.startupTimeoutMs = 5000;
    config.shutdownTimeoutMs = 3000;
    config.healthTimeoutMs = 1000;
    return config;
}

void testWatchdogServiceManager(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dataServiceExe)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "watchdog_dataservice.db";
    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);
    connection.close();

    etfdt::watchdog::ServiceProcessManager manager;
    auto missingStatus = manager.status("missing");
    expectTrue(!missingStatus.hasValue(), "status for missing service returns error");

    const std::string socketName = "ETFDecisionTerminalWatchdogTest_"
        + std::to_string(QCoreApplication::applicationPid());
    auto added = manager.addService(dataServiceConfig(dataServiceExe, dbPath, socketName));
    expectTrue(added.hasValue(), "addService succeeds");
    expectTrue(manager.listStatuses().size() == 1U, "listStatuses returns configured service");

    auto started = manager.startService("ETFDataService");
    expectTrue(started.hasValue(), "startService succeeds");
    auto running = manager.isRunning("ETFDataService");
    expectTrue(running.hasValue() && running.value(), "isRunning returns true");

    auto status = manager.status("ETFDataService");
    expectTrue(status.hasValue(), "status returns configured service");
    if (status) {
        expectTrue(status.value().running, "status running=true");
        expectTrue(status.value().pid > 0, "status has pid");
    }

    bool healthy = waitUntil(
        [&]() {
            auto health = manager.checkHealth("ETFDataService");
            return health.hasValue()
                && health.value().healthState == etfdt::watchdog::HealthState::Healthy;
        },
        5000);
    expectTrue(healthy, "checkHealth returns HEALTHY for DataService");

    auto stopped = manager.stopService("ETFDataService");
    expectTrue(stopped.hasValue(), "stopService succeeds");
    auto stoppedRunning = manager.isRunning("ETFDataService");
    expectTrue(stoppedRunning.hasValue() && !stoppedRunning.value(), "isRunning false after stop");
    auto stoppedStatus = manager.status("ETFDataService");
    expectTrue(
        stoppedStatus.hasValue()
            && stoppedStatus.value().healthState == etfdt::watchdog::HealthState::Stopped,
        "status is STOPPED after stop");

    manager.stopAll();
    auto afterStopAll = manager.isRunning("ETFDataService");
    expectTrue(afterStopAll.hasValue() && !afterStopAll.value(), "stopAll leaves service stopped");

    etfdt::watchdog::ServiceProcessManager missingExeManager;
    auto missingExeConfig = dataServiceConfig(dataServiceExe, dbPath, socketName + "_missing_exe");
    missingExeConfig.serviceName = "MissingExe";
    missingExeConfig.executablePath = (tempDir / "missing.exe").string();
    expectTrue(missingExeManager.addService(missingExeConfig).hasValue(), "missing exe service added");
    auto missingStart = missingExeManager.startService("MissingExe");
    expectTrue(!missingStart.hasValue(), "missing executable start returns error");

    etfdt::watchdog::ServiceProcessManager unhealthyManager;
    const std::string actualSocket = socketName + "_actual";
    auto unhealthyConfig = dataServiceConfig(dataServiceExe, dbPath, actualSocket);
    unhealthyConfig.serviceName = "UnhealthyDataService";
    unhealthyConfig.socketName = socketName + "_not_listening";
    expectTrue(unhealthyManager.addService(unhealthyConfig).hasValue(), "unhealthy service added");
    expectTrue(unhealthyManager.startService("UnhealthyDataService").hasValue(), "unhealthy service starts");
    auto unhealthy = unhealthyManager.checkHealth("UnhealthyDataService");
    expectTrue(unhealthy.hasValue(), "unhealthy health check returns status");
    if (unhealthy) {
        expectTrue(
            unhealthy.value().healthState == etfdt::watchdog::HealthState::Unhealthy,
            "wrong socket health check returns UNHEALTHY");
    }
    expectTrue(unhealthyManager.stopService("UnhealthyDataService").hasValue(), "unhealthy service stops");
    unhealthyManager.stopAll();

    std::filesystem::remove_all(tempDir);
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto migrationPath = optionValue(argc, argv, "--migration");
    const auto dataServiceExe = optionValue(argc, argv, "--dataservice-exe");
    if (migrationPath.empty() || dataServiceExe.empty()) {
        std::cerr << "--migration <path> and --dataservice-exe <path> are required\n";
        return 1;
    }

    testWatchdogServiceManager(migrationPath, dataServiceExe);

    if (gFailures != 0) {
        std::cerr << gFailures << " Watchdog test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Watchdog service manager tests passed." << '\n';
    return 0;
}
