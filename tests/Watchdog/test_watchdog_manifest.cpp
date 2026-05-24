#include "Watchdog/Watchdog.h"

#include <QCoreApplication>
#include <QProcess>

#include <chrono>
#include <filesystem>
#include <fstream>
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
        / ("etfdt_watchdog_manifest_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

std::string validManifestJson()
{
    return R"json({
  "version": "1.0",
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "executablePath": "build/apps/ETFDataService/Debug/ETFDataService.exe",
      "workingDirectory": ".",
      "arguments": ["--serve-readonly", "--db", "data/ETFDecision.db", "--socket-name", "ETFDataServiceReadonly"],
      "socketName": "ETFDataServiceReadonly",
      "startupTimeoutMs": 5000,
      "shutdownTimeoutMs": 3000,
      "healthTimeoutMs": 3000,
      "autoRestart": false
    }
  ]
})json";
}

std::filesystem::path writeFile(
    const std::filesystem::path& directory,
    const std::string& fileName,
    const std::string& content)
{
    const auto path = directory / fileName;
    std::ofstream out(path);
    out << content;
    return path;
}

int runWatchdogCli(
    const std::filesystem::path& watchdogExe,
    const QStringList& arguments,
    QString* output = nullptr)
{
    QProcess process;
    process.setProgram(QString::fromStdWString(watchdogExe.wstring()));
    process.setArguments(arguments);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start();
    if (!process.waitForStarted(5000)) {
        return -1000;
    }
    if (!process.waitForFinished(5000)) {
        process.kill();
        (void)process.waitForFinished(3000);
        return -1001;
    }
    if (output != nullptr) {
        *output = QString::fromUtf8(process.readAllStandardOutput());
    }
    return process.exitCode();
}

void testManifestLoader(const std::filesystem::path& tempDir)
{
    auto loaded = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(validManifestJson());
    expectTrue(loaded.hasValue(), "loadFromJsonString normal config succeeds");
    if (loaded) {
        expectEqual(loaded.value().version, "1.0", "manifest version parsed");
        expectTrue(loaded.value().services.size() == 1U, "manifest service count parsed");
        const auto& service = loaded.value().services.front();
        expectEqual(service.serviceName, "ETFDataService", "serviceName parsed");
        expectTrue(service.enabled, "enabled parsed");
        expectEqual(service.socketName, "ETFDataServiceReadonly", "socketName parsed");
        expectTrue(service.arguments.size() == 5U, "arguments parsed");
        expectEqual(service.arguments.at(0), "--serve-readonly", "argument value parsed");
        expectTrue(service.startupTimeoutMs == 5000, "startupTimeoutMs parsed");
    }

    const auto validPath = writeFile(tempDir, "valid_manifest.json", validManifestJson());
    auto loadedFromFile = etfdt::watchdog::ServiceManifestLoader::loadFromFile(validPath);
    expectTrue(loadedFromFile.hasValue(), "loadFromFile normal config succeeds");

    auto missingVersion = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"services":[]})json");
    expectTrue(!missingVersion.hasValue(), "missing version returns error");

    auto servicesNotArray = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"version":"1.0","services":{}})json");
    expectTrue(!servicesNotArray.hasValue(), "services not array returns error");

    auto missingServiceName = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"version":"1.0","services":[{"executablePath":"ETFDataService.exe","socketName":"s"}]})json");
    expectTrue(!missingServiceName.hasValue(), "missing serviceName returns error");

    auto missingExecutable = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"version":"1.0","services":[{"serviceName":"ETFDataService","socketName":"s"}]})json");
    expectTrue(!missingExecutable.hasValue(), "missing executablePath returns error");

    auto duplicateService = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"version":"1.0","services":[{"serviceName":"ETFDataService","executablePath":"a","socketName":"s"},{"serviceName":"ETFDataService","executablePath":"b","socketName":"s2"}]})json");
    expectTrue(!duplicateService.hasValue(), "duplicate serviceName returns error");

    auto negativeTimeout = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"version":"1.0","services":[{"serviceName":"ETFDataService","executablePath":"a","socketName":"s","startupTimeoutMs":-1}]})json");
    expectTrue(!negativeTimeout.hasValue(), "negative timeout returns error");

    auto disabledFutureService = etfdt::watchdog::ServiceManifestLoader::loadFromJsonString(
        R"json({"version":"1.0","services":[{"serviceName":"ETFMarketService","enabled":false,"executablePath":"future.exe"}]})json");
    expectTrue(disabledFutureService.hasValue(), "disabled future service can be read");
    if (disabledFutureService) {
        const auto validation =
            etfdt::watchdog::ServiceManifestLoader::validate(disabledFutureService.value());
        expectTrue(validation.valid, "disabled future service validates with warning");
        expectTrue(!validation.warnings.empty(), "disabled future service emits warning");
    }
}

void testWatchdogCli(const std::filesystem::path& watchdogExe, const std::filesystem::path& tempDir)
{
    const auto validPath = writeFile(tempDir, "cli_valid_manifest.json", validManifestJson());
    const auto invalidPath = writeFile(
        tempDir,
        "cli_invalid_manifest.json",
        R"json({"version":"1.0","services":[{"serviceName":"ETFDataService","socketName":"s"}]})json");

    QString output;
    const int checkCode = runWatchdogCli(
        watchdogExe,
        {"--check-config", "--config", QString::fromStdWString(validPath.wstring())},
        &output);
    expectTrue(checkCode == 0, "--check-config valid config returns 0");
    expectTrue(output.contains("config: valid"), "--check-config prints valid status");
    expectTrue(output.contains("services: 1"), "--check-config prints service count");

    const int invalidCode = runWatchdogCli(
        watchdogExe,
        {"--check-config", "--config", QString::fromStdWString(invalidPath.wstring())});
    expectTrue(invalidCode != 0, "--check-config invalid config returns non-zero");

    QString listOutput;
    const int listCode = runWatchdogCli(
        watchdogExe,
        {"--list-services", "--config", QString::fromStdWString(validPath.wstring())},
        &listOutput);
    expectTrue(listCode == 0, "--list-services valid config returns 0");
    expectTrue(listOutput.contains("serviceName: ETFDataService"), "--list-services prints serviceName");
    expectTrue(listOutput.contains("enabled: true"), "--list-services prints enabled");
    expectTrue(
        listOutput.contains("socketName: ETFDataServiceReadonly"),
        "--list-services prints socketName");
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto watchdogExe = optionValue(argc, argv, "--watchdog-exe");
    if (watchdogExe.empty()) {
        std::cerr << "--watchdog-exe <path> is required\n";
        return 1;
    }

    const auto tempDir = createTempDirectory();
    testManifestLoader(tempDir);
    testWatchdogCli(watchdogExe, tempDir);
    std::filesystem::remove_all(tempDir);

    if (gFailures != 0) {
        std::cerr << gFailures << " Watchdog manifest test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Watchdog manifest tests passed." << '\n';
    return 0;
}
