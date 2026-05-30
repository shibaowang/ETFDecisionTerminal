#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    (void)invokeSnapshotWrite(*connection);
    const auto duplicate = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(duplicate);
    ok &= expect(duplicate.payload.value("duplicateHandling").toString().toStdString() == "UPSERT_BY_UID",
                 "duplicate handling is UPSERT_BY_UID");
    ok &= expect(countRows(*connection, "position_snapshot") == 1, "duplicate position not inserted");
    return ok ? 0 : 1;
}
