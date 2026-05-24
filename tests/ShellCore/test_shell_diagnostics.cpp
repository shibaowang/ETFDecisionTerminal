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

std::string multiServiceJson()
{
    return R"json({
  "configPath": "config/services.local.example.json",
  "version": "1.0",
  "totalServices": 4,
  "enabledServices": 3,
  "disabledServices": 1,
  "errorCount": 1,
  "warningCount": 1,
  "services": [
    {
      "serviceName": "ETFStrategyService",
      "enabled": true,
      "supported": true,
      "executablePath": "ETFStrategyService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFStrategyServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": true,
      "issues": [
        {"level": "WARNING", "code": "CONFIG_WARNING", "message": "strategy warning message"}
      ]
    },
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
    },
    {
      "serviceName": "ETFMarketService",
      "enabled": true,
      "supported": true,
      "executablePath": "missing.exe",
      "executableExists": false,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFMarketServiceReadonly",
      "socketNamePresent": true,
      "healthCheckSupported": true,
      "canStart": false,
      "issues": [
        {"level": "ERROR", "code": "EXECUTABLE_NOT_FOUND", "message": "market executable missing"},
        {"level": "WARNING", "code": "SECONDARY_WARNING", "message": "secondary market issue"}
      ]
    },
    {
      "serviceName": "ETFAlertService",
      "enabled": false,
      "supported": true,
      "executablePath": "ETFAlertService.exe",
      "executableExists": true,
      "workingDirectory": ".",
      "workingDirectoryExists": true,
      "socketName": "ETFAlertServiceReadonly",
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

void testFilterSortAggregate()
{
    etfdt::shell::ShellDiagnosticFacade facade;
    const auto loaded = facade.loadFromWatchdogReportJson(multiServiceJson());
    expectTrue(loaded.ok, "multi-service report loads");
    const auto& model = loaded.viewModel;
    expectEqual(static_cast<int>(model.serviceRows.size()), 4, "default ViewModel has all services");

    etfdt::shell::ShellDiagnosticFilter noDisabled;
    noDisabled.includeDisabled = false;
    const auto enabledVisible = etfdt::shell::ShellDiagnosticFacade::applyFilter(model, noDisabled);
    expectEqual(static_cast<int>(enabledVisible.serviceRows.size()), 3, "includeDisabled=false removes disabled services");

    etfdt::shell::ShellDiagnosticFilter onlyEnabled;
    onlyEnabled.onlyEnabled = true;
    const auto onlyEnabledModel = etfdt::shell::ShellDiagnosticFacade::applyFilter(model, onlyEnabled);
    expectEqual(static_cast<int>(onlyEnabledModel.serviceRows.size()), 3, "onlyEnabled keeps enabled services");

    etfdt::shell::ShellDiagnosticFilter onlyBlocked;
    onlyBlocked.onlyBlocked = true;
    const auto blockedModel = etfdt::shell::ShellDiagnosticFacade::applyFilter(model, onlyBlocked);
    expectEqual(static_cast<int>(blockedModel.serviceRows.size()), 1, "onlyBlocked keeps blocked enabled service");
    expectEqual(blockedModel.serviceRows.front().serviceName, "ETFMarketService", "blocked service is market");

    etfdt::shell::ShellDiagnosticFilter onlyWithIssues;
    onlyWithIssues.onlyWithIssues = true;
    const auto withIssues = etfdt::shell::ShellDiagnosticFacade::applyFilter(model, onlyWithIssues);
    expectEqual(static_cast<int>(withIssues.serviceRows.size()), 2, "onlyWithIssues keeps services with issues");

    etfdt::shell::ShellDiagnosticFilter searchService;
    searchService.searchText = "data";
    const auto searchServiceModel = etfdt::shell::ShellDiagnosticFacade::applyFilter(model, searchService);
    expectEqual(static_cast<int>(searchServiceModel.serviceRows.size()), 1, "searchText matches serviceName");
    expectEqual(searchServiceModel.serviceRows.front().serviceName, "ETFDataService", "searchText serviceName result");

    etfdt::shell::ShellDiagnosticFilter searchIssue;
    searchIssue.searchText = "executable missing";
    const auto searchIssueModel = etfdt::shell::ShellDiagnosticFacade::applyFilter(model, searchIssue);
    expectEqual(static_cast<int>(searchIssueModel.serviceRows.size()), 1, "searchText matches issue message");
    expectEqual(searchIssueModel.serviceRows.front().serviceName, "ETFMarketService", "searchText issue result");

    etfdt::shell::ShellDiagnosticSort byName;
    byName.sortKey = etfdt::shell::ShellDiagnosticSortKey::ServiceName;
    const auto byNameModel = etfdt::shell::ShellDiagnosticFacade::applySort(model, byName);
    expectEqual(byNameModel.serviceRows.front().serviceName, "ETFAlertService", "SERVICE_NAME ascending sorts alphabetically");

    etfdt::shell::ShellDiagnosticSort bySeverity;
    bySeverity.sortKey = etfdt::shell::ShellDiagnosticSortKey::Severity;
    bySeverity.ascending = false;
    const auto bySeverityModel = etfdt::shell::ShellDiagnosticFacade::applySort(model, bySeverity);
    expectEqual(bySeverityModel.serviceRows.front().statusSeverity, "ERROR", "SEVERITY descending puts ERROR first");

    etfdt::shell::ShellDiagnosticSort byIssues;
    byIssues.sortKey = etfdt::shell::ShellDiagnosticSortKey::IssueCount;
    byIssues.ascending = false;
    const auto byIssuesModel = etfdt::shell::ShellDiagnosticFacade::applySort(model, byIssues);
    expectEqual(byIssuesModel.serviceRows.front().issueCount, 2, "ISSUE_COUNT descending puts highest count first");

    etfdt::shell::ShellDiagnosticSort byEnabled;
    byEnabled.sortKey = etfdt::shell::ShellDiagnosticSortKey::Enabled;
    const auto byEnabledModel = etfdt::shell::ShellDiagnosticFacade::applySort(model, byEnabled);
    expectTrue(byEnabledModel.serviceRows.front().enabled, "ENABLED ascending puts enabled services first");

    etfdt::shell::ShellDiagnosticSort byCanStart;
    byCanStart.sortKey = etfdt::shell::ShellDiagnosticSortKey::CanStart;
    const auto byCanStartModel = etfdt::shell::ShellDiagnosticFacade::applySort(model, byCanStart);
    expectTrue(byCanStartModel.serviceRows.front().canStart, "CAN_START ascending puts startable services first");

    const auto aggregate = etfdt::shell::ShellDiagnosticFacade::aggregate(model);
    expectEqual(aggregate.total, 4, "aggregate total");
    expectEqual(aggregate.enabled, 3, "aggregate enabled");
    expectEqual(aggregate.disabled, 1, "aggregate disabled");
    expectEqual(aggregate.blocked, 1, "aggregate blocked");
    expectEqual(aggregate.withIssues, 2, "aggregate withIssues");
    expectEqual(aggregate.warning, 1, "aggregate warning");
    expectEqual(aggregate.error, 1, "aggregate error");
}

void testRefresh(const std::filesystem::path& tempDir)
{
    etfdt::shell::ShellDiagnosticFacade facade;
    const auto missing = facade.checkReportFile((tempDir / "missing.json").string());
    expectTrue(!missing.exists, "checkReportFile missing file exists=false");

    const auto reportPath = writeFile(tempDir, "refresh.json", validReportJson());
    const auto beforeLoad = facade.checkReportFile(reportPath.string());
    expectTrue(beforeLoad.exists, "checkReportFile existing file exists=true");
    expectTrue(beforeLoad.changedSinceLastLoad, "existing file changed before first load");

    const auto firstRefresh = facade.refreshIfChanged(reportPath.string());
    expectTrue(firstRefresh.ok, "refreshIfChanged first load succeeds");
    expectTrue(firstRefresh.reloaded, "refreshIfChanged first load reloads");
    expectEqual(firstRefresh.viewModel.warningCount, 0, "first refresh valid report");

    const auto secondRefresh = facade.refreshIfChanged(reportPath.string());
    expectTrue(secondRefresh.ok, "refreshIfChanged unchanged file succeeds");
    expectTrue(!secondRefresh.reloaded, "refreshIfChanged unchanged file does not reload");

    {
        std::ofstream out(reportPath);
        out << warningOnlyJson();
    }
    const auto writeTime = std::filesystem::last_write_time(reportPath);
    std::filesystem::last_write_time(reportPath, writeTime + std::chrono::seconds(2));

    const auto changed = facade.checkReportFile(reportPath.string());
    expectTrue(changed.changedSinceLastLoad, "checkReportFile detects changed file");
    const auto thirdRefresh = facade.refreshIfChanged(reportPath.string());
    expectTrue(thirdRefresh.ok, "refreshIfChanged changed file succeeds");
    expectTrue(thirdRefresh.reloaded, "refreshIfChanged changed file reloads");
    expectEqual(thirdRefresh.viewModel.warningCount, 1, "changed refresh loaded warning report");

    {
        std::ofstream out(reportPath);
        out << "{ invalid json";
    }
    const auto invalidWriteTime = std::filesystem::last_write_time(reportPath);
    std::filesystem::last_write_time(reportPath, invalidWriteTime + std::chrono::seconds(2));
    const auto invalidRefresh = facade.refreshIfChanged(reportPath.string());
    expectTrue(!invalidRefresh.ok, "refreshIfChanged invalid JSON returns error");
    expectTrue(invalidRefresh.reloaded, "refreshIfChanged invalid changed file attempts reload");
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
        testFilterSortAggregate();
        testRefresh(tempDir);
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
