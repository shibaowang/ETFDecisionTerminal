#include "ShellServices/ShellAccountingDataServiceAdapter.h"

namespace etfdt::shell_services {

namespace {

constexpr const char* kAdapterNotConnectedStatus =
    "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED";
constexpr const char* kDataQualityUnavailable = "UNAVAILABLE";
constexpr const char* kIssueLevelError = "ERROR";
constexpr const char* kIssueSource = "ShellAccountingDataServiceAdapter";

ShellAccountingIssue makeAdapterNotConnectedIssue()
{
    return {
        kAdapterNotConnectedStatus,
        kIssueLevelError,
        "Shell accounting DataService adapter is a skeleton and has no live client connected.",
        true,
        kIssueSource,
    };
}

ShellAccountingServiceResult makeNotConnectedResult(
    const ShellAccountingServiceRequest& request,
    const char* fallbackActionName)
{
    ShellAccountingServiceResult result;
    result.actionName = request.actionName.empty() ? fallbackActionName : request.actionName;
    result.protocolSuccess = true;
    result.implemented = false;
    result.readOnly = true;
    result.writeEnabled = false;
    result.payloadStatus = kAdapterNotConnectedStatus;
    result.dataQualityStatus = kDataQualityUnavailable;
    result.domainError = true;
    result.issues.push_back(makeAdapterNotConnectedIssue());
    result.generatedTradeDraft = false;
    result.generatedTradeSuggestion = false;
    result.strategyExecuted = false;
    result.brokerOrderSubmitted = false;
    return result;
}

}  // namespace

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchPositionList(
    const ShellAccountingServiceRequest& request)
{
    return makeNotConnectedResult(request, "position.list");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchCashSummary(
    const ShellAccountingServiceRequest& request)
{
    return makeNotConnectedResult(request, "cash.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchPortfolioPnlSummary(
    const ShellAccountingServiceRequest& request)
{
    return makeNotConnectedResult(request, "portfolio.pnl.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchBasePositionSummary(
    const ShellAccountingServiceRequest& request)
{
    return makeNotConnectedResult(request, "base_position.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchSniperPoolSummary(
    const ShellAccountingServiceRequest& request)
{
    return makeNotConnectedResult(request, "sniper_pool.summary");
}

bool ShellAccountingDataServiceAdapter::hasLiveClient() const noexcept
{
    return false;
}

bool ShellAccountingDataServiceAdapter::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingDataServiceAdapter::writeEnabled() const noexcept
{
    return false;
}

}  // namespace etfdt::shell_services
