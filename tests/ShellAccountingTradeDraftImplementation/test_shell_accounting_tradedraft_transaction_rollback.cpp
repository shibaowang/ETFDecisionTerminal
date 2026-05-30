#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto invalid = invokeTradeDraftCreate(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","strategyId":"9999","strategyCode":"TASK139","instrumentId":"1001","instrumentCode":"159509","side":"BUY","quantity1e6":1000000,"reason":"rebalance draft","source":"snapshotRebuildPreview","sourceSnapshotId":"snapshot","riskFlags":{"checked":true,"blocked":false},"authorization":"TASK-148_TRADEDRAFT_WRITE"})");

    bool ok = expect(!invalid.response.success, "invalid strategy rejects TradeDraft action");
    ok &= expect(countRows(*connection, "trade_draft") == 0, "invalid request writes no draft");
    ok &= expect(countRows(*connection, "audit_log") == 0, "invalid request writes no audit");
    return ok ? 0 : 1;
}
