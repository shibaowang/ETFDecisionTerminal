#include "ShellServices/ShellAccountingDataServiceAdapter.h"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

etfdt::shell_services::ShellAccountingServiceRequest requestFor(std::string actionName)
{
    etfdt::shell_services::ShellAccountingServiceRequest request;
    request.actionName = std::move(actionName);
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PF-DEMO-001";
    request.includeIssues = true;
    return request;
}

void expectNotConnected(
    const etfdt::shell_services::ShellAccountingServiceResult& result,
    std::string_view expectedAction)
{
    expectTrue(result.actionName == expectedAction, "actionName is retained");
    expectTrue(result.protocolSuccess, "not connected skeleton uses domain unavailable result");
    expectTrue(!result.implemented, "implemented=false");
    expectTrue(result.readOnly, "readOnly=true");
    expectTrue(!result.writeEnabled, "writeEnabled=false");
    expectTrue(result.payloadStatus == "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED", "payloadStatus is not connected");
    expectTrue(result.dataQualityStatus == "UNAVAILABLE", "dataQualityStatus=UNAVAILABLE");
    expectTrue(result.domainError, "domainError=true");
    expectTrue(!result.protocolError, "protocolError=false");
    expectTrue(!result.generatedTradeDraft, "generatedTradeDraft=false");
    expectTrue(!result.generatedTradeSuggestion, "generatedTradeSuggestion=false");
    expectTrue(!result.strategyExecuted, "strategyExecuted=false");
    expectTrue(!result.brokerOrderSubmitted, "brokerOrderSubmitted=false");
    expectTrue(!result.issues.empty(), "adapter not connected issue is visible");
    if (!result.issues.empty()) {
        expectTrue(
            result.issues.front().code == "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED",
            "issue code is adapter not connected");
        expectTrue(result.issues.front().level == "ERROR", "issue level is ERROR");
        expectTrue(result.issues.front().blocking, "issue is blocking");
        expectTrue(
            result.issues.front().sourceId == "ShellAccountingDataServiceAdapter",
            "issue source is adapter skeleton");
    }
}

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingDataServiceAdapter adapter;

    expectNotConnected(
        adapter.fetchPositionList(requestFor("position.list")),
        "position.list");
    expectNotConnected(
        adapter.fetchCashSummary(requestFor("cash.summary")),
        "cash.summary");
    expectNotConnected(
        adapter.fetchPortfolioPnlSummary(requestFor("portfolio.pnl.summary")),
        "portfolio.pnl.summary");
    expectNotConnected(
        adapter.fetchBasePositionSummary(requestFor("base_position.summary")),
        "base_position.summary");
    expectNotConnected(
        adapter.fetchSniperPoolSummary(requestFor("sniper_pool.summary")),
        "sniper_pool.summary");

    etfdt::shell_services::ShellAccountingServiceRequest emptyActionRequest;
    expectNotConnected(
        adapter.fetchPositionList(emptyActionRequest),
        "position.list");

    return gFailures == 0 ? 0 : 1;
}
