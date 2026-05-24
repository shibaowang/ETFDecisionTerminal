#include "ShellCore/ShellDiagnosticFacade.h"

#include "Diagnostics/WatchdogReportParser.h"

#include <algorithm>
#include <cctype>
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

    return row;
}

}  // namespace

ShellDiagnosticLoadResult ShellDiagnosticFacade::loadFromWatchdogReportFile(const std::string& path)
{
    const auto parsed = diagnostics::WatchdogReportParser::parseFromFile(path);
    if (!parsed.ok) {
        lastError_ = parsed.errorMessage;
        return {false, {}, lastError_};
    }

    const auto summary = diagnostics::WatchdogReportParser::summarize(parsed.report);
    lastError_.clear();
    return {true, buildViewModel(parsed.report, summary), {}};
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

}  // namespace etfdt::shell
