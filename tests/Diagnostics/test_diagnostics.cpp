#include "Diagnostics/WatchdogReportParser.h"

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

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_diagnostics_" + std::to_string(now));
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

void testParsing(const std::filesystem::path& tempDir)
{
    using etfdt::diagnostics::WatchdogReportParser;

    const auto parsed = WatchdogReportParser::parseFromJsonString(validReportJson());
    expectTrue(parsed.ok, "parseFromJsonString valid report succeeds");
    expectEqual(parsed.report.totalServices, 1, "valid report totalServices parsed");
    expectTrue(
        !parsed.report.services.empty()
            && parsed.report.services.front().serviceName == "ETFDataService",
        "valid report serviceName parsed");

    const auto filePath = writeFile(tempDir, "valid.json", validReportJson());
    const auto parsedFile = WatchdogReportParser::parseFromFile(filePath.string());
    expectTrue(parsedFile.ok, "parseFromFile valid report succeeds");

    const auto missingServices = WatchdogReportParser::parseFromJsonString(
        R"json({"version":"1.0","totalServices":0,"enabledServices":0,"disabledServices":0,"errorCount":0,"warningCount":0})json");
    expectTrue(!missingServices.ok, "missing services returns error");

    const auto servicesNotArray = WatchdogReportParser::parseFromJsonString(
        R"json({"version":"1.0","totalServices":0,"enabledServices":0,"disabledServices":0,"errorCount":0,"warningCount":0,"services":{}})json");
    expectTrue(!servicesNotArray.ok, "services not array returns error");

    const auto boolWrong = WatchdogReportParser::parseFromJsonString(
        R"json({"version":"1.0","totalServices":1,"enabledServices":1,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[{"serviceName":"ETFDataService","enabled":"true","supported":true,"executablePath":"x","executableExists":true,"workingDirectory":".","workingDirectoryExists":true,"socketName":"s","socketNamePresent":true,"healthCheckSupported":true,"canStart":true,"issues":[]}]})json");
    expectTrue(!boolWrong.ok, "bool field type error returns error");

    const auto numberWrong = WatchdogReportParser::parseFromJsonString(
        R"json({"version":"1.0","totalServices":"1","enabledServices":1,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[]})json");
    expectTrue(!numberWrong.ok, "number field type error returns error");

    const auto issuesWrong = WatchdogReportParser::parseFromJsonString(
        R"json({"version":"1.0","totalServices":1,"enabledServices":1,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[{"serviceName":"ETFDataService","enabled":true,"supported":true,"executablePath":"x","executableExists":true,"workingDirectory":".","workingDirectoryExists":true,"socketName":"s","socketNamePresent":true,"healthCheckSupported":true,"canStart":true,"issues":{}}]})json");
    expectTrue(!issuesWrong.ok, "issues not array returns error");

    const auto emptyServices = WatchdogReportParser::parseFromJsonString(
        R"json({"version":"1.0","totalServices":0,"enabledServices":0,"disabledServices":0,"errorCount":0,"warningCount":0,"services":[]})json");
    expectTrue(!emptyServices.ok, "empty services is treated as invalid report");
}

void testSummary()
{
    using etfdt::diagnostics::WatchdogReportParser;

    const auto valid = WatchdogReportParser::parseFromJsonString(validReportJson());
    const auto validSummary = WatchdogReportParser::summarize(valid.report);
    expectTrue(validSummary.ok, "valid summary ok=true");
    expectEqual(validSummary.exitCode, 0, "valid summary exitCode=0");
    expectTrue(
        validSummary.text.find("ETFDataService") != std::string::npos,
        "summary text contains service name");

    const auto error = WatchdogReportParser::parseFromJsonString(errorReportJson());
    const auto errorSummary = WatchdogReportParser::summarize(error.report);
    expectTrue(!errorSummary.ok, "errorCount > 0 summary ok=false");
    expectEqual(errorSummary.exitCode, 1, "errorCount > 0 summary exitCode=1");

    const auto cannotStart = WatchdogReportParser::parseFromJsonString(cannotStartJson());
    const auto cannotStartSummary = WatchdogReportParser::summarize(cannotStart.report);
    expectTrue(!cannotStartSummary.ok, "enabled canStart=false summary ok=false");
    expectEqual(cannotStartSummary.enabledBlockedCount, 1, "blocked enabled count is tracked");

    const auto warning = WatchdogReportParser::parseFromJsonString(warningOnlyJson());
    const auto warningSummary = WatchdogReportParser::summarize(warning.report);
    expectTrue(warningSummary.ok, "warning-only summary ok=true");
    expectEqual(warningSummary.exitCode, 0, "warning-only summary exitCode=0");
    expectEqual(warningSummary.warningCount, 1, "warning-only summary warningCount=1");
}

}  // namespace

int main()
{
    const auto tempDir = createTempDirectory();
    try {
        testParsing(tempDir);
        testSummary();
    } catch (const std::exception& ex) {
        ++gFailures;
        std::cerr << "FAILED: unexpected exception: " << ex.what() << '\n';
    }
    std::filesystem::remove_all(tempDir);

    if (gFailures != 0) {
        std::cerr << gFailures << " Diagnostics test failure(s)\n";
        return 1;
    }
    return 0;
}
