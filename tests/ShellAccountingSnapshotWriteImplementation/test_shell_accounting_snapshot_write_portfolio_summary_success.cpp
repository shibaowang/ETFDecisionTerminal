#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(countRows(*connection, "portfolio_summary") == 1, "one portfolio summary row written");
    ok &= expect(querySingleInt64(*connection, "SELECT holding_cost_cents FROM portfolio_summary;") == 120000,
                 "holding cost persisted from preview positions");
    ok &= expect(querySingleInt64(*connection, "SELECT cash_balance_cents FROM portfolio_summary;") == 880000,
                 "portfolio cash persisted from preview cash");
    return ok ? 0 : 1;
}
