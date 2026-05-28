#include "ShellAccountingDataServiceAdapterLiveCallGate.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <string_view>
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

bool contains(const std::vector<std::string>& values, const std::string& value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_live_call_gate;

    const auto allowlist = liveCallReadOnlyAllowlist();
    expectTrue(allowlist.size() == 5, "allowlist has exactly five actions");

    expectTrue(isLiveCallAllowedAction("position.list"), "position.list allowed");
    expectTrue(isLiveCallAllowedAction("cash.summary"), "cash.summary allowed");
    expectTrue(isLiveCallAllowedAction("portfolio.pnl.summary"), "portfolio.pnl.summary allowed");
    expectTrue(isLiveCallAllowedAction("base_position.summary"), "base_position.summary allowed");
    expectTrue(isLiveCallAllowedAction("sniper_pool.summary"), "sniper_pool.summary allowed");

    for (const auto& wrapper : allowlist) {
        expectTrue(wrapper.readOnly, "allowlisted wrapper is read-only");
        expectTrue(wrapper.wrapperMethod.rfind("DataServiceClient::", 0) == 0, "allowlisted wrapper names DataServiceClient guard wrapper");
    }

    const auto denylist = liveCallForbiddenDenylist();
    expectTrue(contains(denylist, "data.audit.append"), "data.audit.append denied");
    expectTrue(contains(denylist, "trade.write"), "trade write denied");
    expectTrue(contains(denylist, "trade_draft.create"), "draft denied");
    expectTrue(contains(denylist, "snapshot.write"), "snapshot write denied");
    expectTrue(contains(denylist, "strategy.execute"), "strategy denied");
    expectTrue(contains(denylist, "broker.order.submit"), "broker denied");

    expectTrue(!isLiveCallAllowedAction("data.audit.append"), "data.audit.append not allowed");
    expectTrue(!isLiveCallAllowedAction("trade.write"), "trade write not allowed");
    expectTrue(!isLiveCallAllowedAction("trade_draft.create"), "draft not allowed");
    expectTrue(!isLiveCallAllowedAction("snapshot.write"), "snapshot write not allowed");
    expectTrue(!isLiveCallAllowedAction("strategy.execute"), "strategy not allowed");
    expectTrue(!isLiveCallAllowedAction("broker.order.submit"), "broker not allowed");

    return gFailures == 0 ? 0 : 1;
}
