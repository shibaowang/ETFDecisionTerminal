#include "ShellCore/ShellDiagnosticFacade.h"

#include "Diagnostics/WatchdogReportParser.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <sstream>
#include <utility>

namespace etfdt::shell {
namespace {

std::string upperAscii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

int severityRankForLevel(const std::string& level)
{
    const auto upper = upperAscii(level);
    if (upper == "ERROR" || upper == "CRITICAL" || upper == "FATAL") {
        return 3;
    }
    if (upper == "WARNING") {
        return 2;
    }
    if (upper == "INFO") {
        return 1;
    }
    return 1;
}

int severityRankForStatus(const std::string& statusSeverity)
{
    const auto upper = upperAscii(statusSeverity);
    if (upper == "OK") {
        return 0;
    }
    if (upper == "DISABLED") {
        return 1;
    }
    if (upper == "WARNING") {
        return 2;
    }
    if (upper == "ERROR") {
        return 3;
    }
    return 4;
}

std::string lowerAscii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

bool containsCaseInsensitive(const std::string& haystack, const std::string& needle)
{
    if (needle.empty()) {
        return true;
    }
    return lowerAscii(haystack).find(lowerAscii(needle)) != std::string::npos;
}

ShellDiagnosticIssueRow buildIssueRow(const diagnostics::DiagnosticIssue& issue)
{
    ShellDiagnosticIssueRow row;
    row.level = issue.level;
    row.code = issue.code;
    row.message = issue.message;
    row.severityRank = severityRankForLevel(issue.level);
    row.displayText = issue.level + " " + issue.code + " - " + issue.message;
    return row;
}

ShellDiagnosticServiceRow buildServiceRow(const diagnostics::DiagnosticServiceStatus& service)
{
    ShellDiagnosticServiceRow row;
    row.serviceName = service.serviceName;
    row.enabled = service.enabled;
    row.supported = service.supported;
    row.canStart = service.canStart;
    row.healthCheckSupported = service.healthCheckSupported;
    row.executableExists = service.executableExists;
    row.workingDirectoryExists = service.workingDirectoryExists;
    row.socketNamePresent = service.socketNamePresent;
    row.issueCount = static_cast<int>(service.issues.size());
    row.issues.reserve(service.issues.size());

    int maxIssueSeverity = 0;
    for (const auto& issue : service.issues) {
        auto issueRow = buildIssueRow(issue);
        maxIssueSeverity = std::max(maxIssueSeverity, issueRow.severityRank);
        row.issues.push_back(std::move(issueRow));
    }

    if (!service.enabled) {
        row.statusSeverity = "DISABLED";
        row.statusText = "Disabled";
    } else if (!service.canStart) {
        row.statusSeverity = "ERROR";
        row.statusText = "Cannot start";
    } else if (!service.supported || maxIssueSeverity == 2) {
        row.statusSeverity = "WARNING";
        row.statusText = !service.supported ? "Unsupported service" : "Warnings present";
    } else if (maxIssueSeverity >= 3) {
        row.statusSeverity = "ERROR";
        row.statusText = "Errors present";
    } else {
        row.statusSeverity = "OK";
        row.statusText = "Ready";
    }
    row.statusSeverityRank = severityRankForStatus(row.statusSeverity);

    return row;
}

bool matchesFilter(const ShellDiagnosticServiceRow& row, const ShellDiagnosticFilter& filter)
{
    if (!filter.includeDisabled && !row.enabled) {
        return false;
    }
    if (filter.onlyEnabled && !row.enabled) {
        return false;
    }
    if (filter.onlyBlocked && !(row.enabled && !row.canStart)) {
        return false;
    }
    if (filter.onlyWithIssues && row.issueCount == 0) {
        return false;
    }
    if (row.statusSeverityRank < severityRankForStatus(filter.minimumSeverity)) {
        return false;
    }
    if (!filter.searchText.empty()) {
        bool matched = containsCaseInsensitive(row.serviceName, filter.searchText)
            || containsCaseInsensitive(row.statusText, filter.searchText);
        for (const auto& issue : row.issues) {
            matched = matched || containsCaseInsensitive(issue.message, filter.searchText)
                || containsCaseInsensitive(issue.code, filter.searchText)
                || containsCaseInsensitive(issue.displayText, filter.searchText);
        }
        if (!matched) {
            return false;
        }
    }
    return true;
}

void recalculateViewModelCounts(ShellDiagnosticViewModel& viewModel)
{
    viewModel.totalServices = static_cast<int>(viewModel.serviceRows.size());
    viewModel.enabledServices = 0;
    viewModel.disabledServices = 0;
    viewModel.errorCount = 0;
    viewModel.warningCount = 0;
    viewModel.blockedServiceCount = 0;

    for (const auto& row : viewModel.serviceRows) {
        if (row.enabled) {
            ++viewModel.enabledServices;
        } else {
            ++viewModel.disabledServices;
        }
        if (row.enabled && !row.canStart) {
            ++viewModel.blockedServiceCount;
        }
        if (row.statusSeverity == "ERROR") {
            ++viewModel.errorCount;
        } else if (row.statusSeverity == "WARNING") {
            ++viewModel.warningCount;
        }
    }
    viewModel.ok = viewModel.errorCount == 0 && viewModel.blockedServiceCount == 0;
}

template <typename Value>
bool compareValues(const Value& left, const Value& right, bool ascending)
{
    if (left == right) {
        return false;
    }
    return ascending ? left < right : right < left;
}

bool compareBoolTrueFirst(bool left, bool right, bool ascending)
{
    if (left == right) {
        return false;
    }
    return ascending ? left > right : right > left;
}

std::string fileTimeString(const std::filesystem::file_time_type& value)
{
    return std::to_string(value.time_since_epoch().count());
}

}  // namespace

ShellDiagnosticLoadResult ShellDiagnosticFacade::loadFromWatchdogReportFile(const std::string& path)
{
    return loadViewModelFromFile(path, {}, {});
}

ShellDiagnosticLoadResult ShellDiagnosticFacade::loadViewModelFromFile(
    const std::string& path,
    const ShellDiagnosticFilter& filter,
    const ShellDiagnosticSort& sort)
{
    const auto parsed = diagnostics::WatchdogReportParser::parseFromFile(path);
    if (!parsed.ok) {
        lastError_ = parsed.errorMessage;
        updateRefreshState(path, false, lastError_, {});
        return {false, {}, lastError_};
    }

    const auto summary = diagnostics::WatchdogReportParser::summarize(parsed.report);
    auto viewModel = buildViewModel(parsed.report, summary, filter, sort);
    lastError_.clear();
    updateRefreshState(path, true, {}, viewModel);
    return {true, viewModel, {}};
}

ShellDiagnosticLoadResult ShellDiagnosticFacade::loadFromWatchdogReportJson(const std::string& json)
{
    const auto parsed = diagnostics::WatchdogReportParser::parseFromJsonString(json);
    if (!parsed.ok) {
        lastError_ = parsed.errorMessage;
        return {false, {}, lastError_};
    }

    const auto summary = diagnostics::WatchdogReportParser::summarize(parsed.report);
    lastError_.clear();
    return {true, buildViewModel(parsed.report, summary), {}};
}

const std::string& ShellDiagnosticFacade::lastError() const
{
    return lastError_;
}

ShellDiagnosticViewModel ShellDiagnosticFacade::buildViewModel(
    const diagnostics::DiagnosticReport& report,
    const diagnostics::DiagnosticSummary& summary)
{
    ShellDiagnosticViewModel viewModel;
    viewModel.ok = summary.ok;
    viewModel.title = summary.title;
    viewModel.summaryText = summary.text;
    viewModel.totalServices = report.totalServices;
    viewModel.enabledServices = report.enabledServices;
    viewModel.disabledServices = report.disabledServices;
    viewModel.errorCount = report.errorCount;
    viewModel.warningCount = report.warningCount;
    viewModel.blockedServiceCount = summary.enabledBlockedCount;
    viewModel.serviceRows.reserve(report.services.size());

    for (const auto& service : report.services) {
        viewModel.serviceRows.push_back(buildServiceRow(service));
    }

    return viewModel;
}

ShellDiagnosticViewModel ShellDiagnosticFacade::buildViewModel(
    const diagnostics::DiagnosticReport& report,
    const diagnostics::DiagnosticSummary& summary,
    const ShellDiagnosticFilter& filter,
    const ShellDiagnosticSort& sort)
{
    return applySort(applyFilter(buildViewModel(report, summary), filter), sort);
}

ShellDiagnosticViewModel ShellDiagnosticFacade::applyFilter(
    const ShellDiagnosticViewModel& viewModel,
    const ShellDiagnosticFilter& filter)
{
    ShellDiagnosticViewModel filtered = viewModel;
    filtered.serviceRows.clear();
    filtered.serviceRows.reserve(viewModel.serviceRows.size());
    for (const auto& row : viewModel.serviceRows) {
        if (matchesFilter(row, filter)) {
            filtered.serviceRows.push_back(row);
        }
    }
    recalculateViewModelCounts(filtered);
    return filtered;
}

ShellDiagnosticViewModel ShellDiagnosticFacade::applySort(
    const ShellDiagnosticViewModel& viewModel,
    const ShellDiagnosticSort& sort)
{
    ShellDiagnosticViewModel sorted = viewModel;
    std::stable_sort(sorted.serviceRows.begin(), sorted.serviceRows.end(), [&](const auto& left, const auto& right) {
        switch (sort.sortKey) {
        case ShellDiagnosticSortKey::ServiceName:
            return compareValues(lowerAscii(left.serviceName), lowerAscii(right.serviceName), sort.ascending);
        case ShellDiagnosticSortKey::Severity:
            return compareValues(left.statusSeverityRank, right.statusSeverityRank, sort.ascending);
        case ShellDiagnosticSortKey::CanStart:
            return compareBoolTrueFirst(left.canStart, right.canStart, sort.ascending);
        case ShellDiagnosticSortKey::Enabled:
            return compareBoolTrueFirst(left.enabled, right.enabled, sort.ascending);
        case ShellDiagnosticSortKey::IssueCount:
            return compareValues(left.issueCount, right.issueCount, sort.ascending);
        }
        return false;
    });
    return sorted;
}

ShellDiagnosticAggregate ShellDiagnosticFacade::aggregate(const ShellDiagnosticViewModel& viewModel)
{
    ShellDiagnosticAggregate aggregate;
    aggregate.total = static_cast<int>(viewModel.serviceRows.size());
    for (const auto& row : viewModel.serviceRows) {
        if (row.enabled) {
            ++aggregate.enabled;
        } else {
            ++aggregate.disabled;
        }
        if (row.statusSeverity == "OK") {
            ++aggregate.ok;
        } else if (row.statusSeverity == "WARNING") {
            ++aggregate.warning;
        } else if (row.statusSeverity == "ERROR") {
            ++aggregate.error;
        }
        if (row.enabled && !row.canStart) {
            ++aggregate.blocked;
        }
        if (row.issueCount > 0) {
            ++aggregate.withIssues;
        }
    }
    return aggregate;
}

ShellDiagnosticRefreshState ShellDiagnosticFacade::checkReportFile(const std::string& path) const
{
    ShellDiagnosticRefreshState state;
    state.reportPath = path;
    state.lastLoadSucceeded = lastLoadSucceeded_;
    state.lastError = lastError_;

    std::error_code ec;
    state.exists = std::filesystem::exists(path, ec) && !ec;
    if (!state.exists) {
        state.changedSinceLastLoad = hasLastReportWriteTime_ || path != lastReportPath_;
        return state;
    }

    const auto writeTime = std::filesystem::last_write_time(path, ec);
    if (ec) {
        state.exists = false;
        state.lastError = ec.message();
        state.changedSinceLastLoad = true;
        return state;
    }

    state.lastModifiedUtc = fileTimeString(writeTime);
    state.changedSinceLastLoad = !hasLastReportWriteTime_ || path != lastReportPath_
        || writeTime != lastReportWriteTime_;
    return state;
}

ShellDiagnosticRefreshResult ShellDiagnosticFacade::refreshIfChanged(const std::string& path)
{
    auto state = checkReportFile(path);
    if (!state.exists) {
        lastError_ = "Report file does not exist: " + path;
        lastLoadSucceeded_ = false;
        state.lastLoadSucceeded = false;
        state.lastError = lastError_;
        return {false, false, state, {}, lastError_};
    }

    if (!state.changedSinceLastLoad) {
        return {lastLoadSucceeded_, false, state, lastViewModel_, lastError_};
    }

    auto loaded = loadFromWatchdogReportFile(path);
    state = checkReportFile(path);
    return {loaded.ok, true, state, loaded.viewModel, loaded.errorMessage};
}

void ShellDiagnosticFacade::updateRefreshState(
    const std::string& path,
    bool loadSucceeded,
    const std::string& errorMessage,
    const ShellDiagnosticViewModel& viewModel)
{
    lastReportPath_ = path;
    lastLoadSucceeded_ = loadSucceeded;
    lastViewModel_ = viewModel;
    if (!errorMessage.empty()) {
        lastError_ = errorMessage;
    }

    std::error_code ec;
    if (std::filesystem::exists(path, ec) && !ec) {
        const auto writeTime = std::filesystem::last_write_time(path, ec);
        if (!ec) {
            lastReportWriteTime_ = writeTime;
            hasLastReportWriteTime_ = true;
        }
    }
}

}  // namespace etfdt::shell
