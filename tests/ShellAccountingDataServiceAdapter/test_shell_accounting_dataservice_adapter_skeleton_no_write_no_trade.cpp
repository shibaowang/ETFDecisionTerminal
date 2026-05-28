#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingServiceTypes.h"

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

void expectNoWriteNoTrade(
    const etfdt::shell_services::ShellAccountingServiceResult& result,
    std::string_view message)
{
    expectTrue(
        etfdt::shell_services::isShellAccountingServiceResultReadOnly(result),
        message);
    expectTrue(!result.generatedTradeDraft, "result does not generate TradeDraft");
    expectTrue(!result.generatedTradeSuggestion, "result does not generate trade suggestion");
    expectTrue(!result.strategyExecuted, "result does not execute strategy");
    expectTrue(!result.brokerOrderSubmitted, "result does not submit broker order");
    expectTrue(result.rawPayload.find("data.audit.append") == std::string::npos, "result does not invoke audit append");
    expectTrue(result.rawPayload.find("trade.") == std::string::npos, "result does not invoke trade write action");
    expectTrue(result.rawPayload.find("draft") == std::string::npos, "result does not invoke draft action");
    expectTrue(result.rawPayload.find("snapshot") == std::string::npos, "result does not invoke snapshot write");
}

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingDataServiceAdapter adapter;
    etfdt::shell_services::ShellAccountingServiceRequest request;

    expectNoWriteNoTrade(adapter.fetchPositionList(request), "position.list skeleton is no-write/no-trade");
    expectNoWriteNoTrade(adapter.fetchCashSummary(request), "cash.summary skeleton is no-write/no-trade");
    expectNoWriteNoTrade(adapter.fetchPortfolioPnlSummary(request), "portfolio.pnl.summary skeleton is no-write/no-trade");
    expectNoWriteNoTrade(adapter.fetchBasePositionSummary(request), "base_position.summary skeleton is no-write/no-trade");
    expectNoWriteNoTrade(adapter.fetchSniperPoolSummary(request), "sniper_pool.summary skeleton is no-write/no-trade");

    expectTrue(adapter.readOnly(), "adapter readOnly=true");
    expectTrue(!adapter.writeEnabled(), "adapter writeEnabled=false");
    expectTrue(!adapter.hasLiveClient(), "adapter has no live client");

    return gFailures == 0 ? 0 : 1;
}
