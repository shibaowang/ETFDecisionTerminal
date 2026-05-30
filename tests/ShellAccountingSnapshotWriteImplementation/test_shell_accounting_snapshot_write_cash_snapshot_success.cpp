#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(countRows(*connection, "cash_snapshot") == 1, "one cash snapshot row written");
    ok &= expect(querySingleInt64(*connection, "SELECT cash_balance_cents FROM cash_snapshot;") == 880000,
                 "cash balance persisted from replay preview");
    return ok ? 0 : 1;
}
