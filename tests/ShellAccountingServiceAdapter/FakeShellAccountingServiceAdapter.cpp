#include "FakeShellAccountingServiceAdapter.h"

#include <utility>

namespace etfdt::tests::shell_accounting_service_adapter {

FakeShellAccountingServiceAdapter::FakeShellAccountingServiceAdapter(
    etfdt::shell_services::ShellAccountingServiceResult result)
    : result_(std::move(result))
{
}

etfdt::shell_services::ShellAccountingServiceResult
FakeShellAccountingServiceAdapter::fetchPositionList(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return makeResult(request, "position.list");
}

etfdt::shell_services::ShellAccountingServiceResult
FakeShellAccountingServiceAdapter::fetchCashSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return makeResult(request, "cash.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
FakeShellAccountingServiceAdapter::fetchPortfolioPnlSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return makeResult(request, "portfolio.pnl.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
FakeShellAccountingServiceAdapter::fetchBasePositionSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return makeResult(request, "base_position.summary");
}

etfdt::shell_services::ShellAccountingServiceResult
FakeShellAccountingServiceAdapter::fetchSniperPoolSummary(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    return makeResult(request, "sniper_pool.summary");
}

int FakeShellAccountingServiceAdapter::callCount() const noexcept
{
    return callCount_;
}

const std::string& FakeShellAccountingServiceAdapter::lastActionName() const noexcept
{
    return lastActionName_;
}

etfdt::shell_services::ShellAccountingServiceResult
FakeShellAccountingServiceAdapter::makeResult(
    const etfdt::shell_services::ShellAccountingServiceRequest& request,
    std::string actionName)
{
    ++callCount_;
    lastActionName_ = std::move(actionName);
    auto result = result_;
    result.actionName = lastActionName_;
    if (!request.actionName.empty()) {
        result.rawPayload = request.actionName;
    }
    return result;
}

etfdt::shell_services::ShellAccountingServiceRequest makeDemoServiceRequest(
    std::string actionName)
{
    etfdt::shell_services::ShellAccountingServiceRequest request;
    request.actionName = std::move(actionName);
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PF-DEMO-001";
    request.sourceFromTime = "2026-01-01T00:00:00Z";
    request.sourceToTime = "2026-01-31T23:59:59Z";
    request.includeIssues = true;
    request.includeDebugMetadata = true;
    request.calculationVersion = "accounting-replay-v0";
    request.requestedOutputs = {"positions", "issues"};
    request.timeoutMs = 5000;
    return request;
}

etfdt::shell_services::ShellAccountingServiceResult makeGuardUnavailableResult(
    std::string actionName,
    std::string status)
{
    etfdt::shell_services::ShellAccountingServiceResult result;
    result.actionName = std::move(actionName);
    result.protocolSuccess = true;
    result.implemented = false;
    result.readOnly = true;
    result.writeEnabled = false;
    result.payloadStatus = std::move(status);
    result.dataQualityStatus = "UNAVAILABLE";
    result.domainError = true;
    result.issues.push_back({
        result.payloadStatus,
        "ERROR",
        "Guard response is not implemented.",
        true,
        result.actionName,
    });
    result.errors = result.issues;
    return result;
}

}  // namespace etfdt::tests::shell_accounting_service_adapter
