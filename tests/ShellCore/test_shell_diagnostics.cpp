#include "ShellCore/ShellCore.h"

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

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_shell_diagnostics_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
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

std::string validReportJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 0,
  "warningCount": 0,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": []
    }
  ]
})json";
}

std::string warningOnlyJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 0,
  "warningCount": 1,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": [
        {"level": "WARNING", "code": "AUTO_RESTART_IGNORED", "message": "autoRestart ignored"}
      ]
    }
  ]
})json";
}

std::string errorReportJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 1,
  "warningCount": 0,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "missing.exe",
      "executableExists": false,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": [
        {"level": "ERROR", "code": "EXECUTABLE_NOT_FOUND", "message": "missing executable"}
      ]
    }
  ]
})json";
}

std::string disabledReportJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 0,
  "disabledServices": 1,
  "errorCount": 0,
  "warningCount": 0,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": false,
      "supported": true,
      "executablePath": "ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": []
    }
  ]
})json";
}

std::string cannotStartJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 1,
  "enabledServices": 1,
  "disabledServices": 0,
  "errorCount": 0,
  "warningCount": 0,
  "services": [
    {
      "serviceName": "ETFDataService",
      "enabled": true,
      "supported": true,
      "executablePath": "ETFDataService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFDataServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": []
    }
  ]
})json";
}

void testValidInputs(const std::filesystem::path& tempDir)
{
    etfdt::shell::ShellDiagnosticFacade facade;

    const auto filePath = writeFile(tempDir, "valid.json", validReportJson());
    const auto fromFile = facade.loadFromWatchdogReportFile(filePath.string());
    expectTrue(fromFile.ok, "valid Watchdog JSON file loads");
    expectTrue(fromFile.viewModel.ok, "valid file ViewModel ok=true");
    expectEqual(fromFile.viewModel.totalServices, 1, "totalServices copied");
    expectEqual(fromFile.viewModel.enabledServices, 1, "enabledServices copied");
    expectEqual(fromFile.viewModel.blockedServiceCount, 0, "blockedServiceCount is zero");
    expectTrue(!fromFile.viewModel.summaryText.empty(), "summaryText is non-empty");
    expectTrue(!fromFile.viewModel.serviceRows.empty(), "serviceRows populated");
    expectEqual(
        fromFile.viewModel.serviceRows.front().serviceName,
        "ETFDataService",
        "service row contains ETFDataService");
    expectEqual(fromFile.viewModel.serviceRows.front().statusSeverity, "OK", "ready service OK");

    const auto fromJson = facade.loadFromWatchdogReportJson(validReportJson());
    expectTrue(fromJson.ok, "valid Watchdog JSON string loads");
    expectTrue(fromJson.viewModel.ok, "valid JSON string ViewModel ok=true");
}

void testStates()
{
    etfdt::shell::ShellDiagnosticFacade facade;

    const auto warning = facade.loadFromWatchdogReportJson(warningOnlyJson());
    expectTrue(warning.ok, "warning-only report loads");
    expectTrue(warning.viewModel.ok, "warning-only ViewModel ok=true");
    expectEqual(warning.viewModel.warningCount, 1, "warningCount copied");
    expectEqual(
        warning.viewModel.serviceRows.front().statusSeverity,
        "WARNING",
        "warning issue maps to WARNING severity");
    expectTrue(
        warning.viewModel.summaryText.find("AUTO_RESTART_IGNORED") != std::string::npos,
        "warning summary contains issue");

    const auto error = facade.loadFromWatchdogReportJson(errorReportJson());
    expectTrue(error.ok, "error report still parses");
    expectTrue(!error.viewModel.ok, "errorCount > 0 ViewModel ok=false");
    expectEqual(error.viewModel.errorCount, 1, "errorCount copied");
    expectEqual(error.viewModel.blockedServiceCount, 1, "error report blocked count");
    expectEqual(
        error.viewModel.serviceRows.front().statusSeverity,
        "ERROR",
        "cannot start error maps to ERROR severity");

    const auto blocked = facade.loadFromWatchdogReportJson(cannotStartJson());
    expectTrue(blocked.ok, "canStart=false report parses");
    expectTrue(!blocked.viewModel.ok, "enabled canStart=false ViewModel ok=false");
    expectEqual(blocked.viewModel.blockedServiceCount, 1, "blockedServiceCount is tracked");

    const auto disabled = facade.loadFromWatchdogReportJson(disabledReportJson());
    expectTrue(disabled.ok, "disabled service report parses");
    expectTrue(disabled.viewModel.ok, "disabled service alone does not fail ViewModel");
    expectEqual(
        disabled.viewModel.serviceRows.front().statusSeverity,
        "DISABLED",
        "disabled service maps to DISABLED severity");
}

void testFailures()
{
    etfdt::shell::ShellDiagnosticFacade facade;

    const auto invalidJson = facade.loadFromWatchdogReportJson("{ invalid json");
    expectTrue(!invalidJson.ok, "invalid JSON returns error");
    expectTrue(!facade.lastError().empty(), "lastError populated after invalid JSON");

    const auto missingFields = facade.loadFromWatchdogReportJson(
        R"json({"version":"1.0","totalServices":0,"enabledServices":0,"disabledServices":0,"errorCount":0,"warningCount":0})json");
    expectTrue(!missingFields.ok, "missing fields returns error");
    expectTrue(!missingFields.errorMessage.empty(), "missing fields error message returned");
}

}  // namespace

int main()
{
    const auto tempDir = createTempDirectory();
    try {
        testValidInputs(tempDir);
        testStates();
        testFailures();
    } catch (const std::exception& ex) {
        ++gFailures;
        std::cerr << "FAILED: unexpected exception: " << ex.what() << '\n';
    }
    std::filesystem::remove_all(tempDir);

    if (gFailures != 0) {
        std::cerr << gFailures << " ShellCore test failure(s)\n";
        return 1;
    }
    return 0;
}
