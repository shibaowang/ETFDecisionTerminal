#include "ShellAccountingDataServiceAdapterExpectedCall.h"

#include <algorithm>

namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding {

std::vector<ShellAccountingDataServiceAdapterExpectedCall> expectedReadOnlyAdapterCalls()
{
    return {
        {"fetchPositionList", "positionList", "position.list", true},
        {"fetchCashSummary", "cashSummary", "cash.summary", true},
        {"fetchPortfolioPnlSummary", "portfolioPnlSummary", "portfolio.pnl.summary", true},
        {"fetchBasePositionSummary", "basePositionSummary", "base_position.summary", true},
        {"fetchSniperPoolSummary", "sniperPoolSummary", "sniper_pool.summary", true},
    };
}

std::vector<std::string> readOnlyActionAllowlist()
{
    std::vector<std::string> actions;
    for (const auto& call : expectedReadOnlyAdapterCalls()) {
        actions.push_back(call.actionName);
    }
    return actions;
}

std::vector<std::string> forbiddenActionDenylist()
{
    return {
        "data.audit.append",
        "trade.create",
        "trade.update",
        "trade.delete",
        "trade_draft.create",
        "snapshot.write",
        "strategy.execute",
        "broker.order.submit",
    };
}

bool isReadOnlyAccountingActionAllowed(const std::string& actionName)
{
    const auto actions = readOnlyActionAllowlist();
    return std::find(actions.begin(), actions.end(), actionName) != actions.end();
}

}  // namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding
