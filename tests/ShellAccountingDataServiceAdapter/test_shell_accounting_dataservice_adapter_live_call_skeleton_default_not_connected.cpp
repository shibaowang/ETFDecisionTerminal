#include "ShellServices/ShellAccountingDataServiceAdapter.h"

#include <iostream>
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

void expectNotConnected(
    const etfdt::shell_services::ShellAccountingServiceResult& result,
    std::string_view actionName)
{
    expectTrue(result.actionName == actionName, "actionName is retained");
    expectTrue(!result.implemented, "implemented=false");
    expectTrue(result.readOnly, "readOnly=true");
    expectTrue(!result.writeEnabled, "writeEnabled=false");
    expectTrue(
        result.payloadStatus == "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED",
        "payloadStatus is not connected");
    expectTrue(result.dataQualityStatus == "UNAVAILABLE", "dataQualityStatus is UNAVAILABLE");
    expectTrue(!result.issues.empty(), "not connected issue is visible");
    if (!result.issues.empty()) {
        expectTrue(result.issues.front().blocking, "not connected issue blocks");
    }
    expectTrue(!result.generatedTradeDraft, "no TradeDraft");
    expectTrue(!result.generatedTradeSuggestion, "no trade suggestion");
}

etfdt::shell_services::ShellAccountingServiceRequest requestFor(std::string_view actionName)
{
    etfdt::shell_services::ShellAccountingServiceRequest request;
    request.actionName = std::string(actionName);
    return request;
}

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingDataServiceAdapter adapter;
    expectTrue(!adapter.hasClientPort(), "default hasClientPort=false");

    expectNotConnected(adapter.fetchPositionList(requestFor("position.list")), "position.list");
    expectNotConnected(adapter.fetchCashSummary(requestFor("cash.summary")), "cash.summary");
    expectNotConnected(
        adapter.fetchPortfolioPnlSummary(requestFor("portfolio.pnl.summary")),
        "portfolio.pnl.summary");
    expectNotConnected(
        adapter.fetchBasePositionSummary(requestFor("base_position.summary")),
        "base_position.summary");
    expectNotConnected(
        adapter.fetchSniperPoolSummary(requestFor("sniper_pool.summary")),
        "sniper_pool.summary");

    return gFailures == 0 ? 0 : 1;
}
