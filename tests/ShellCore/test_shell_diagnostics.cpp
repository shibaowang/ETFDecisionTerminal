#include "ShellCore/ShellCore.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <string_view>

#include <QString>

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

void testPresenterMocks()
{
    etfdt::shell::ShellDiagnosticPresenter presenter;
    expectTrue(presenter.loadMockHealthy(), "Presenter loadMockHealthy succeeds");
    expectTrue(presenter.currentState().loaded, "Presenter healthy state loaded");
    expectTrue(presenter.currentViewModel().ok, "Presenter healthy ViewModel ok=true");
    expectEqual(presenter.currentAggregate().total, 1, "Presenter healthy aggregate total");

    expectTrue(presenter.loadMockWithWarnings(), "Presenter loadMockWithWarnings succeeds");
    expectTrue(presenter.currentViewModel().ok, "Presenter warning mock ok=true");
    expectTrue(presenter.currentViewModel().warningCount > 0, "Presenter warning mock warningCount");

    expectTrue(presenter.loadMockWithErrors(), "Presenter loadMockWithErrors succeeds");
    expectTrue(!presenter.currentViewModel().ok, "Presenter error mock ok=false");
    expectTrue(presenter.currentAggregate().blocked > 0, "Presenter error mock blocked aggregate");

    const auto refresh = presenter.refreshIfChanged();
    expectTrue(!refresh.ok, "Presenter refreshIfChanged on mock returns clear failure");
    expectTrue(!presenter.lastError().empty(), "Presenter mock refresh lastError populated");
}

void testPresenterLoadAndState(const std::filesystem::path& tempDir)
{
    etfdt::shell::ShellDiagnosticPresenter presenter;

    expectTrue(presenter.loadFromJson(validReportJson()), "Presenter loadFromJson valid succeeds");
    expectTrue(presenter.currentState().loaded, "Presenter JSON state loaded");
    expectEqual(presenter.currentViewModel().totalServices, 1, "Presenter JSON totalServices");

    expectTrue(!presenter.loadFromJson("{ invalid json"), "Presenter invalid JSON fails");
    expectTrue(presenter.currentState().hasError, "Presenter invalid JSON hasError");
    expectTrue(!presenter.lastError().empty(), "Presenter invalid JSON lastError");

    const auto reportPath = writeFile(tempDir, "presenter_file.json", validReportJson());
    expectTrue(presenter.loadFromFile(reportPath.string()), "Presenter loadFromFile valid succeeds");
    expectEqual(presenter.currentState().sourcePath, reportPath.string(), "Presenter sourcePath tracked");

    expectTrue(!presenter.loadFromFile((tempDir / "missing_presenter.json").string()), "Presenter missing file fails");
    expectTrue(presenter.currentState().hasError, "Presenter missing file hasError");
}

void testPresenterFilterSortAggregate()
{
    etfdt::shell::ShellDiagnosticPresenter presenter;
    expectTrue(presenter.loadMockMixed(), "Presenter loadMockMixed succeeds");
    expectEqual(presenter.currentAggregate().total, 4, "Presenter mixed aggregate total");

    etfdt::shell::ShellDiagnosticFilter blocked;
    blocked.onlyBlocked = true;
    presenter.setFilter(blocked);
    expectEqual(static_cast<int>(presenter.currentViewModel().serviceRows.size()), 1, "Presenter onlyBlocked filter");
    expectEqual(presenter.currentViewModel().serviceRows.front().serviceName, "ETFStrategyService", "Presenter blocked service");

    etfdt::shell::ShellDiagnosticFilter search;
    search.searchText = "delayed market";
    presenter.setFilter(search);
    expectEqual(static_cast<int>(presenter.currentViewModel().serviceRows.size()), 1, "Presenter search filter");
    expectEqual(presenter.currentViewModel().serviceRows.front().serviceName, "ETFMarketService", "Presenter search issue result");

    presenter.clearFilter();
    expectEqual(static_cast<int>(presenter.currentViewModel().serviceRows.size()), 4, "Presenter clearFilter restores rows");

    etfdt::shell::ShellDiagnosticSort byName;
    byName.sortKey = etfdt::shell::ShellDiagnosticSortKey::ServiceName;
    presenter.setSort(byName);
    expectEqual(presenter.currentViewModel().serviceRows.front().serviceName, "ETFAlertService", "Presenter sort by service name");

    etfdt::shell::ShellDiagnosticSort bySeverity;
    bySeverity.sortKey = etfdt::shell::ShellDiagnosticSortKey::Severity;
    bySeverity.ascending = false;
    presenter.setSort(bySeverity);
    expectEqual(presenter.currentViewModel().serviceRows.front().statusSeverity, "ERROR", "Presenter sort by severity");
    expectEqual(presenter.currentAggregate().blocked, 1, "Presenter aggregate after sort");
}

void testPresenterRefresh(const std::filesystem::path& tempDir)
{
    etfdt::shell::ShellDiagnosticPresenter presenter;
    const auto reportPath = writeFile(tempDir, "presenter_refresh.json", validReportJson());
    expectTrue(presenter.loadFromFile(reportPath.string()), "Presenter refresh setup load");

    const auto unchanged = presenter.refreshIfChanged();
    expectTrue(unchanged.ok, "Presenter refresh unchanged ok");
    expectTrue(!unchanged.reloaded, "Presenter refresh unchanged does not reload");

    {
        std::ofstream out(reportPath);
        out << warningOnlyJson();
    }
    const auto writeTime = std::filesystem::last_write_time(reportPath);
    std::filesystem::last_write_time(reportPath, writeTime + std::chrono::seconds(2));

    const auto changed = presenter.refreshIfChanged();
    expectTrue(changed.ok, "Presenter refresh changed ok");
    expectTrue(changed.reloaded, "Presenter refresh changed reloads");
    expectEqual(presenter.currentViewModel().warningCount, 1, "Presenter refresh changed warning loaded");
}

void testQtServiceListModelRoles()
{
    etfdt::shell::ShellDiagnosticQtAdapter adapter;
    expectTrue(adapter.loadMockHealthy(), "QtAdapter loadMockHealthy succeeds");

    auto* serviceModel = adapter.serviceModel();
    expectTrue(serviceModel->rowCount() > 0, "ServiceListModel has rows");

    const auto roles = serviceModel->roleNames();
    expectTrue(
        roles.contains(etfdt::shell::ShellDiagnosticServiceListModel::ServiceNameRole),
        "ServiceListModel has serviceName role");
    expectEqual(
        std::string(roles.value(etfdt::shell::ShellDiagnosticServiceListModel::ServiceNameRole).constData()),
        "serviceName",
        "ServiceListModel serviceName role name");
    expectTrue(
        roles.contains(etfdt::shell::ShellDiagnosticServiceListModel::StatusSeverityRole),
        "ServiceListModel has statusSeverity role");
    expectTrue(
        roles.contains(etfdt::shell::ShellDiagnosticServiceListModel::CanStartRole),
        "ServiceListModel has canStart role");

    const auto first = serviceModel->index(0, 0);
    expectEqual(
        serviceModel
            ->data(first, etfdt::shell::ShellDiagnosticServiceListModel::ServiceNameRole)
            .toString()
            .toStdString(),
        "ETFDataService",
        "ServiceListModel serviceName data");
    expectEqual(
        serviceModel
            ->data(first, etfdt::shell::ShellDiagnosticServiceListModel::StatusSeverityRole)
            .toString()
            .toStdString(),
        "OK",
        "ServiceListModel statusSeverity data");
    expectTrue(
        serviceModel
            ->data(first, etfdt::shell::ShellDiagnosticServiceListModel::CanStartRole)
            .toBool(),
        "ServiceListModel canStart data");
}

void testQtIssueListModelRoles()
{
    etfdt::shell::ShellDiagnosticQtAdapter adapter;
    expectTrue(adapter.loadMockHealthy(), "QtAdapter issue healthy load succeeds");
    expectTrue(adapter.selectService(0), "QtAdapter selects healthy service");
    expectEqual(adapter.issueModel()->rowCount(), 0, "IssueListModel empty for no issues");

    expectTrue(adapter.loadMockWithWarnings(), "QtAdapter issue warning load succeeds");
    expectTrue(adapter.selectService(0), "QtAdapter selects warning service");
    auto* issueModel = adapter.issueModel();
    expectTrue(issueModel->rowCount() > 0, "IssueListModel has warning issue");

    const auto roles = issueModel->roleNames();
    expectTrue(
        roles.contains(etfdt::shell::ShellDiagnosticIssueListModel::LevelRole),
        "IssueListModel has level role");
    expectTrue(
        roles.contains(etfdt::shell::ShellDiagnosticIssueListModel::DisplayTextRole),
        "IssueListModel has displayText role");

    const auto first = issueModel->index(0, 0);
    expectEqual(
        issueModel->data(first, etfdt::shell::ShellDiagnosticIssueListModel::LevelRole)
            .toString()
            .toStdString(),
        "WARNING",
        "IssueListModel level data");
    expectTrue(
        issueModel->data(first, etfdt::shell::ShellDiagnosticIssueListModel::SeverityRankRole)
            .toInt()
            > 0,
        "IssueListModel severityRank data");
}

void testQtSummaryAndAdapter(const std::filesystem::path& tempDir)
{
    etfdt::shell::ShellDiagnosticQtAdapter adapter;
    expectTrue(adapter.loadMockWithWarnings(), "QtAdapter warning load succeeds");
    expectTrue(adapter.summaryObject()->ok(), "SummaryObject warning ok=true");
    expectEqual(adapter.summaryObject()->totalServices(), 1, "SummaryObject totalServices");
    expectEqual(adapter.summaryObject()->enabledServices(), 1, "SummaryObject enabledServices");
    expectEqual(adapter.summaryObject()->warningCount(), 1, "SummaryObject warningCount");
    expectEqual(adapter.summaryObject()->errorCount(), 0, "SummaryObject errorCount");

    expectTrue(adapter.loadMockWithErrors(), "QtAdapter error load succeeds");
    expectTrue(!adapter.summaryObject()->ok(), "SummaryObject error ok=false");
    expectEqual(adapter.summaryObject()->errorCount(), 1, "SummaryObject error count");

    expectTrue(adapter.loadMockMixed(), "QtAdapter mixed load succeeds");
    expectEqual(adapter.serviceModel()->rowCount(), 4, "QtAdapter mixed rowCount");

    etfdt::shell::ShellDiagnosticFilter blocked;
    blocked.onlyBlocked = true;
    adapter.setFilter(blocked);
    expectEqual(adapter.serviceModel()->rowCount(), 1, "QtAdapter onlyBlocked rowCount");
    expectEqual(
        adapter.serviceModel()
            ->data(adapter.serviceModel()->index(0, 0), etfdt::shell::ShellDiagnosticServiceListModel::ServiceNameRole)
            .toString()
            .toStdString(),
        "ETFStrategyService",
        "QtAdapter onlyBlocked service");

    adapter.clearFilter();
    etfdt::shell::ShellDiagnosticSort byName;
    byName.sortKey = etfdt::shell::ShellDiagnosticSortKey::ServiceName;
    adapter.setSort(byName);
    expectEqual(
        adapter.serviceModel()
            ->data(adapter.serviceModel()->index(0, 0), etfdt::shell::ShellDiagnosticServiceListModel::ServiceNameRole)
            .toString()
            .toStdString(),
        "ETFAlertService",
        "QtAdapter sort by service name");

    etfdt::shell::ShellDiagnosticSort bySeverity;
    bySeverity.sortKey = etfdt::shell::ShellDiagnosticSortKey::Severity;
    bySeverity.ascending = false;
    adapter.setSort(bySeverity);
    expectEqual(
        adapter.serviceModel()
            ->data(adapter.serviceModel()->index(0, 0), etfdt::shell::ShellDiagnosticServiceListModel::StatusSeverityRole)
            .toString()
            .toStdString(),
        "ERROR",
        "QtAdapter sort by severity");
    expectTrue(adapter.selectService(0), "QtAdapter select error service");
    expectTrue(adapter.issueModel()->rowCount() > 0, "QtAdapter selectService populates issues");

    const auto reportPath = writeFile(tempDir, "qt_adapter_valid.json", validReportJson());
    expectTrue(
        adapter.loadFromFile(QString::fromStdString(reportPath.string())),
        "QtAdapter loadFromFile valid succeeds");
    expectEqual(adapter.serviceModel()->rowCount(), 1, "QtAdapter loadFromFile rowCount");

    expectTrue(
        !adapter.loadFromFile(QString::fromStdString((tempDir / "missing_qt_adapter.json").string())),
        "QtAdapter loadFromFile missing fails");
    expectTrue(!adapter.lastError().isEmpty(), "QtAdapter missing file lastError");

    expectTrue(adapter.loadMockHealthy(), "QtAdapter refresh mock setup");
    const auto refresh = adapter.refreshIfChanged();
    expectTrue(!refresh.ok, "QtAdapter refreshIfChanged on mock returns failure");
    expectTrue(!adapter.lastError().isEmpty(), "QtAdapter refresh mock lastError");
}

void testShellPageRegistry()
{
    const auto items = etfdt::shell::ShellPageRegistry::listNavigationItems();
    expectEqual(static_cast<int>(items.size()), 13, "ShellPageRegistry returns 13 navigation items");

    std::set<std::string> keys;
    for (const auto& item : items) {
        expectTrue(keys.insert(item.key).second, "ShellPageRegistry keys are unique");
    }

    const auto diagnostics = etfdt::shell::ShellPageRegistry::metadataForKey("diagnostics");
    expectTrue(diagnostics.has_value(), "diagnostics metadata exists");
    expectEqual(diagnostics->qmlComponent, "DiagnosticsMockPage", "diagnostics points to DiagnosticsMockPage");
    expectTrue(!diagnostics->placeholder, "diagnostics is the only real mock data page");

    const auto dashboard = etfdt::shell::ShellPageRegistry::metadataForKey("dashboard");
    expectTrue(dashboard.has_value(), "dashboard metadata exists");
    expectTrue(dashboard->placeholder, "dashboard is placeholder");

    const auto unknown = etfdt::shell::ShellPageRegistry::metadataForKey("unknown");
    expectTrue(!unknown.has_value(), "unknown metadata returns nullopt");
    const auto fallback = etfdt::shell::ShellPageRegistry::fallbackMetadata("unknown");
    expectEqual(fallback.key, "unknown", "fallback keeps requested key");
    expectTrue(fallback.placeholder, "fallback is placeholder");
}

void testShellNavigationModel()
{
    etfdt::shell::ShellNavigationModel model;
    expectEqual(model.rowCount(), 13, "ShellNavigationModel rowCount is 13");

    const auto roles = model.roleNames();
    expectTrue(roles.contains(etfdt::shell::ShellNavigationModel::KeyRole), "NavigationModel has key role");
    expectTrue(roles.contains(etfdt::shell::ShellNavigationModel::TitleRole), "NavigationModel has title role");
    expectTrue(roles.contains(etfdt::shell::ShellNavigationModel::EnabledRole), "NavigationModel has enabled role");
    expectTrue(roles.contains(etfdt::shell::ShellNavigationModel::PlaceholderRole), "NavigationModel has placeholder role");
    expectEqual(
        std::string(roles.value(etfdt::shell::ShellNavigationModel::KeyRole).constData()),
        "key",
        "NavigationModel key role name");

    const auto first = model.index(0, 0);
    expectEqual(
        model.data(first, etfdt::shell::ShellNavigationModel::KeyRole).toString().toStdString(),
        "dashboard",
        "NavigationModel first key is dashboard");
    expectTrue(
        model.data(first, etfdt::shell::ShellNavigationModel::PlaceholderRole).toBool(),
        "NavigationModel dashboard is placeholder");
}

void testShellNavigationController()
{
    etfdt::shell::ShellNavigationController controller;
    expectEqual(controller.currentPageKey().toStdString(), "dashboard", "NavigationController default key");
    expectTrue(controller.navigationModel() != nullptr, "NavigationController has navigation model");
    expectEqual(controller.navigationModel()->rowCount(), 13, "NavigationController navigation model rowCount");

    expectTrue(controller.selectPage(QStringLiteral("diagnostics")), "selectPage diagnostics succeeds");
    expectEqual(controller.currentPageKey().toStdString(), "diagnostics", "current key is diagnostics");
    expectEqual(controller.currentPageTitle().toStdString(), "诊断中心", "current title is diagnostics");
    expectEqual(
        controller.currentPageQmlComponent().toStdString(),
        "DiagnosticsMockPage",
        "diagnostics loads DiagnosticsMockPage");
    expectTrue(!controller.currentPagePlaceholder(), "diagnostics is not placeholder");

    expectTrue(!controller.selectPage(QStringLiteral("unknown")), "selectPage unknown returns false");
    expectEqual(controller.currentPageKey().toStdString(), "diagnostics", "unknown selection keeps current page");

    expectTrue(controller.selectPageByIndex(1), "selectPageByIndex succeeds");
    expectEqual(controller.currentPageKey().toStdString(), "market", "selectPageByIndex selects sorted model item");
    expectTrue(!controller.selectPageByIndex(999), "selectPageByIndex invalid returns false");
}

void testShellPageStatusModel()
{
    etfdt::shell::ShellPageStatusModel model;
    expectEqual(model.rowCount(), 13, "ShellPageStatusModel rowCount is 13");

    const auto roles = model.roleNames();
    expectTrue(roles.contains(etfdt::shell::ShellPageStatusModel::PageKeyRole), "StatusModel has pageKey role");
    expectTrue(roles.contains(etfdt::shell::ShellPageStatusModel::ModuleStatusRole), "StatusModel has moduleStatus role");
    expectTrue(roles.contains(etfdt::shell::ShellPageStatusModel::DataModeRole), "StatusModel has dataMode role");
    expectTrue(roles.contains(etfdt::shell::ShellPageStatusModel::ConnectionStatusRole), "StatusModel has connectionStatus role");

    const auto* diagnostics = model.statusForKey(QStringLiteral("diagnostics"));
    expectTrue(diagnostics != nullptr, "diagnostics page status exists");
    expectEqual(diagnostics == nullptr ? "" : diagnostics->moduleStatus, "MOCK", "diagnostics moduleStatus=MOCK");
    expectEqual(diagnostics == nullptr ? "" : diagnostics->dataMode, "MOCK_DATA", "diagnostics dataMode=MOCK_DATA");

    const auto* dashboard = model.statusForKey(QStringLiteral("dashboard"));
    expectTrue(dashboard != nullptr, "dashboard page status exists");
    expectEqual(dashboard == nullptr ? "" : dashboard->moduleStatus, "PLACEHOLDER", "dashboard moduleStatus=PLACEHOLDER");
    expectTrue(dashboard != nullptr && dashboard->placeholder, "dashboard status is placeholder");
}

void testShellPageInfoAndStatusController()
{
    etfdt::shell::ShellStatusController controller;
    expectTrue(controller.statusModel() != nullptr, "StatusController has status model");
    expectTrue(controller.pageInfo() != nullptr, "StatusController has pageInfo");
    expectTrue(controller.logModel() != nullptr, "StatusController has logModel");
    expectEqual(controller.statusModel()->rowCount(), 13, "StatusController status rowCount");
    expectEqual(controller.pageInfo()->pageKey().toStdString(), "dashboard", "StatusController default page");
    expectEqual(controller.pageInfo()->moduleStatus().toStdString(), "PLACEHOLDER", "StatusController default module status");

    expectTrue(controller.updateCurrentPageStatus(QStringLiteral("diagnostics")), "StatusController updates diagnostics");
    expectEqual(controller.pageInfo()->pageKey().toStdString(), "diagnostics", "pageInfo diagnostics key");
    expectEqual(controller.pageInfo()->moduleStatus().toStdString(), "MOCK", "pageInfo diagnostics status");
    expectTrue(!controller.pageInfo()->placeholder(), "diagnostics pageInfo not placeholder");

    expectTrue(controller.updateCurrentPageStatus(QStringLiteral("market")), "StatusController updates market");
    expectEqual(controller.pageInfo()->pageKey().toStdString(), "market", "pageInfo market key");
    expectEqual(controller.pageInfo()->moduleStatus().toStdString(), "PLACEHOLDER", "pageInfo market status");
    expectTrue(controller.pageInfo()->placeholder(), "market pageInfo placeholder");

    expectTrue(!controller.updateCurrentPageStatus(QStringLiteral("unknown")), "StatusController unknown page fails");
}

void testShellMockLogModel()
{
    etfdt::shell::ShellMockLogModel model;
    expectTrue(model.rowCount() > 0, "ShellMockLogModel has default rows");

    const auto roles = model.roleNames();
    expectTrue(roles.contains(etfdt::shell::ShellMockLogModel::LevelRole), "MockLogModel has level role");
    expectTrue(roles.contains(etfdt::shell::ShellMockLogModel::SourceRole), "MockLogModel has source role");
    expectTrue(roles.contains(etfdt::shell::ShellMockLogModel::MessageRole), "MockLogModel has message role");

    const int before = model.rowCount();
    model.appendMockLog(QStringLiteral("INFO"), QStringLiteral("Test"), QStringLiteral("Page changed"));
    expectEqual(model.rowCount(), before + 1, "MockLogModel appends page log");
    const auto appended = model.index(before, 0);
    expectEqual(
        model.data(appended, etfdt::shell::ShellMockLogModel::MessageRole).toString().toStdString(),
        "Page changed",
        "MockLogModel appended message");
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
        testPresenterMocks();
        testPresenterLoadAndState(tempDir);
        testPresenterFilterSortAggregate();
        testPresenterRefresh(tempDir);
        testQtServiceListModelRoles();
        testQtIssueListModelRoles();
        testQtSummaryAndAdapter(tempDir);
        testShellPageRegistry();
        testShellNavigationModel();
        testShellNavigationController();
        testShellPageStatusModel();
        testShellPageInfoAndStatusController();
        testShellMockLogModel();
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
