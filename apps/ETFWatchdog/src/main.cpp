#include "Watchdog/Watchdog.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QThread>

#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

namespace {

void printHelp()
{
    std::cout
        << "ETFWatchdog service process manager utility\n"
        << "\n"
        << "Usage:\n"
        << "  ETFWatchdog --demo-start-dataservice --dataservice-exe <path> --db <path> "
           "--socket-name <name>\n"
        << "  ETFWatchdog --help\n"
        << "\n"
        << "This is a development process-management skeleton. It starts a local "
        << "DataService process, checks system.ping and data.health through "
        << "DataServiceClient, then stops the child process.\n";
}

std::string optionValue(const std::vector<std::string>& args, const std::string& option)
{
    for (std::size_t i = 0; i + 1 < args.size(); ++i) {
        if (args[i] == option) {
            return args[i + 1];
        }
    }
    return {};
}

bool hasOption(const std::vector<std::string>& args, const std::string& option)
{
    for (const auto& arg : args) {
        if (arg == option) {
            return true;
        }
    }
    return false;
}

bool processEventsUntil(const std::function<bool()>& predicate, int timeoutMs)
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

int demoStartDataService(
    const std::string& dataServiceExe,
    const std::string& dbPath,
    const std::string& socketName)
{
    if (dataServiceExe.empty() || dbPath.empty() || socketName.empty()) {
        std::cerr << "--dataservice-exe, --db and --socket-name are required\n";
        return 1;
    }

    etfdt::watchdog::ServiceProcessConfig config;
    config.serviceName = "ETFDataService";
    config.executablePath = dataServiceExe;
    config.arguments = {"--serve-readonly", "--db", dbPath, "--socket-name", socketName};
    config.socketName = socketName;
    config.workingDirectory = std::filesystem::current_path().string();
    config.startupTimeoutMs = 5000;
    config.healthTimeoutMs = 1000;
    config.shutdownTimeoutMs = 3000;

    etfdt::watchdog::ServiceProcessManager manager;
    auto added = manager.addService(config);
    if (!added) {
        std::cerr << "Failed to add service: " << added.error().message << '\n';
        return 1;
    }
    auto started = manager.startService(config.serviceName);
    if (!started) {
        std::cerr << "Failed to start service: " << started.error().message << '\n';
        return 1;
    }

    etfdt::watchdog::ServiceProcessStatus status;
    const bool healthy = processEventsUntil(
        [&]() {
            auto health = manager.checkHealth(config.serviceName);
            if (health) {
                status = health.value();
                return status.healthState == etfdt::watchdog::HealthState::Healthy;
            }
            return false;
        },
        config.startupTimeoutMs);

    std::cout << "serviceName: " << status.serviceName << '\n';
    std::cout << "running: " << (status.running ? "true" : "false") << '\n';
    std::cout << "pid: " << status.pid << '\n';
    std::cout << "healthState: " << etfdt::watchdog::toString(status.healthState) << '\n';
    std::cout << "message: " << status.message << '\n';

    auto stopped = manager.stopService(config.serviceName);
    if (!stopped) {
        std::cerr << "Failed to stop service: " << stopped.error().message << '\n';
        return 1;
    }
    return healthy ? 0 : 1;
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }

    if (args.empty() || hasOption(args, "--help")) {
        printHelp();
        return 0;
    }

    if (hasOption(args, "--demo-start-dataservice")) {
        return demoStartDataService(
            optionValue(args, "--dataservice-exe"),
            optionValue(args, "--db"),
            optionValue(args, "--socket-name"));
    }

    printHelp();
    return 1;
}
