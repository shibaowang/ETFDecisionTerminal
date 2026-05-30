#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto zero = invokeTradeDraftCreate(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","strategyId":"1001","strategyCode":"TASK139","instrumentId":"1001","instrumentCode":"159509","side":"BUY","quantity1e6":0,"reason":"rebalance draft","source":"snapshotRebuildPreview","sourceSnapshotId":"snapshot","riskFlags":{"checked":true,"blocked":false},"authorization":"TASK-148_TRADEDRAFT_WRITE"})");
    const auto negative = invokeTradeDraftCreate(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","strategyId":"1001","strategyCode":"TASK139","instrumentId":"1001","instrumentCode":"159509","side":"BUY","quantity1e6":-1,"reason":"rebalance draft","source":"snapshotRebuildPreview","sourceSnapshotId":"snapshot","riskFlags":{"checked":true,"blocked":false},"authorization":"TASK-148_TRADEDRAFT_WRITE"})");

    bool ok = expect(!zero.response.success, "zero quantity rejected");
    ok &= expect(!negative.response.success, "negative quantity rejected");
    ok &= expect(countRows(*connection, "trade_draft") == 0, "invalid quantity writes no draft");
    return ok ? 0 : 1;
}
