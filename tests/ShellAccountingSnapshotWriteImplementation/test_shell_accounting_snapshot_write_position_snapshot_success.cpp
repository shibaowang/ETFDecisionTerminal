#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(countRows(*connection, "position_snapshot") == 1, "one position snapshot row written");
    ok &= expect(querySingleInt64(*connection, "SELECT quantity_1e6 FROM position_snapshot;") == 1000000,
                 "position quantity persisted from replay preview");
    ok &= expect(querySingleInt64(*connection, "SELECT cost_amount_cents FROM position_snapshot;") == 120000,
                 "position cost persisted from replay preview");
    return ok ? 0 : 1;
}
