#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto first = invokeSnapshotWrite(*connection);
    const auto second = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(first);
    ok &= expectSuccessfulSnapshotWrite(second);
    ok &= expect(countRows(*connection, "cash_snapshot") == 1, "cash snapshot remains one row");
    ok &= expect(countRows(*connection, "position_snapshot") == 1, "position snapshot remains one row");
    ok &= expect(countRows(*connection, "portfolio_summary") == 1, "portfolio summary remains one row");
    return ok ? 0 : 1;
}
