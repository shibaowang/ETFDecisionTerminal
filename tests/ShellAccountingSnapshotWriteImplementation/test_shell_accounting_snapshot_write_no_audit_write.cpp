#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(countRows(*connection, "audit_log") == 0, "audit_log remains empty");
    return ok ? 0 : 1;
}
