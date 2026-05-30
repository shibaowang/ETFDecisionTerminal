#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "portfolio.pnl.summary");
    bool ok = expectReplayPayload(payload, "portfolio.pnl.summary", "missing price replay");
    ok &= expect(hasIssue(payload, "MARKET_PRICE_MISSING"), "missing market price issue is mapped");
    return ok ? 0 : 1;
}
