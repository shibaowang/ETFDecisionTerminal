#include "SpyShellAccountingDataServiceClientPort.h"

#include <numeric>
#include <utility>

namespace etfdt::shell_services::tests {

void SpyShellAccountingDataServiceClientPort::setNextResponse(
    ShellAccountingDataServiceClientResponse response)
{
    nextResponse_ = std::move(response);
}

ShellAccountingDataServiceClientResponse SpyShellAccountingDataServiceClientPort::callPositionList(
    const ShellAccountingDataServiceClientRequest& request)
{
    return record("callPositionList", request);
}

ShellAccountingDataServiceClientResponse SpyShellAccountingDataServiceClientPort::callCashSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    return record("callCashSummary", request);
}

ShellAccountingDataServiceClientResponse
SpyShellAccountingDataServiceClientPort::callPortfolioPnlSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    return record("callPortfolioPnlSummary", request);
}

ShellAccountingDataServiceClientResponse
SpyShellAccountingDataServiceClientPort::callBasePositionSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    return record("callBasePositionSummary", request);
}

ShellAccountingDataServiceClientResponse
SpyShellAccountingDataServiceClientPort::callSniperPoolSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    return record("callSniperPoolSummary", request);
}

int SpyShellAccountingDataServiceClientPort::callCount(const std::string& methodName) const
{
    const auto found = callCounts_.find(methodName);
    return found == callCounts_.end() ? 0 : found->second;
}

int SpyShellAccountingDataServiceClientPort::totalCallCount() const
{
    return std::accumulate(
        callCounts_.begin(),
        callCounts_.end(),
        0,
        [](int total, const auto& entry) { return total + entry.second; });
}

const ShellAccountingDataServiceClientRequest&
SpyShellAccountingDataServiceClientPort::lastRequest() const
{
    return lastRequest_;
}

const std::vector<std::string>&
SpyShellAccountingDataServiceClientPort::calledMethods() const noexcept
{
    return calledMethods_;
}

void SpyShellAccountingDataServiceClientPort::dataAuditAppend()
{
    forbiddenActionCalled_ = true;
}

void SpyShellAccountingDataServiceClientPort::tradeWriteAction()
{
    forbiddenActionCalled_ = true;
}

void SpyShellAccountingDataServiceClientPort::draftAction()
{
    forbiddenActionCalled_ = true;
    generatedTradeDraft_ = true;
}

void SpyShellAccountingDataServiceClientPort::snapshotWriteAction()
{
    forbiddenActionCalled_ = true;
}

void SpyShellAccountingDataServiceClientPort::strategyAction()
{
    forbiddenActionCalled_ = true;
    strategyExecuted_ = true;
}

void SpyShellAccountingDataServiceClientPort::brokerAction()
{
    forbiddenActionCalled_ = true;
    brokerOrderSubmitted_ = true;
}

bool SpyShellAccountingDataServiceClientPort::forbiddenActionCalled() const noexcept
{
    return forbiddenActionCalled_;
}

bool SpyShellAccountingDataServiceClientPort::generatedTradeDraft() const noexcept
{
    return generatedTradeDraft_;
}

bool SpyShellAccountingDataServiceClientPort::generatedTradeSuggestion() const noexcept
{
    return generatedTradeSuggestion_;
}

bool SpyShellAccountingDataServiceClientPort::strategyExecuted() const noexcept
{
    return strategyExecuted_;
}

bool SpyShellAccountingDataServiceClientPort::brokerOrderSubmitted() const noexcept
{
    return brokerOrderSubmitted_;
}

ShellAccountingDataServiceClientResponse SpyShellAccountingDataServiceClientPort::record(
    std::string methodName,
    const ShellAccountingDataServiceClientRequest& request)
{
    lastRequest_ = request;
    calledMethods_.push_back(methodName);
    ++callCounts_[methodName];
    return nextResponse_;
}

ShellAccountingServiceRequest makeLiveCallSkeletonRequest(std::string actionName)
{
    ShellAccountingServiceRequest request;
    request.actionName = std::move(actionName);
    request.accountId = "ACC-001";
    request.portfolioId = "PF-001";
    request.sourceFromTime = "2026-01-01T00:00:00Z";
    request.sourceToTime = "2026-01-31T23:59:59Z";
    request.includeIssues = true;
    request.includeDebugMetadata = true;
    request.calculationVersion = "calc-v1";
    request.requestedOutputs = {"positions", "issues"};
    request.timeoutMs = 1500;
    return request;
}

ShellAccountingDataServiceClientResponse makeOkClientResponse(std::string actionName)
{
    ShellAccountingDataServiceClientResponse response;
    response.actionName = std::move(actionName);
    response.protocolSuccess = true;
    response.implemented = true;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus = "OK";
    response.dataQualityStatus = "OK";
    response.hasRows = true;
    response.rawPayload = "{\"status\":\"OK\"}";
    return response;
}

ShellAccountingIssue makePortIssue(
    std::string code,
    std::string level,
    bool blocking)
{
    return {
        std::move(code),
        std::move(level),
        "test issue",
        blocking,
        "SpyShellAccountingDataServiceClientPort",
    };
}

}  // namespace etfdt::shell_services::tests
