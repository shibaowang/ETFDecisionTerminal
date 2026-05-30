#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto before = countRows(*connection, "trade_log");
    const auto call = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(countRows(*connection, "trade_log") == before, "trade_log count unchanged");
    return ok ? 0 : 1;
}
