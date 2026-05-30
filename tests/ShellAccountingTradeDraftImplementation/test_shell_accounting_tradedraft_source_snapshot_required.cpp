#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto missingSourceId = invokeTradeDraftCreate(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","strategyId":"1001","strategyCode":"TASK139","instrumentId":"1001","instrumentCode":"159509","side":"BUY","quantity1e6":1000000,"reason":"rebalance draft","source":"snapshotRebuildPreview","riskFlags":{"checked":true,"blocked":false},"authorization":"TASK-148_TRADEDRAFT_WRITE"})");

    bool ok = expect(!missingSourceId.response.success, "source snapshot/replay id required");
    ok &= expect(countRows(*connection, "trade_draft") == 0, "missing source writes no draft");
    return ok ? 0 : 1;
}
