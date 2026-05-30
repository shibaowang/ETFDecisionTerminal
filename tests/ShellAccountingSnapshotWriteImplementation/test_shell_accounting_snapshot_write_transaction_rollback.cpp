#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    etfdt::data_access::ShellAccountingSnapshotWriteBatch batch;
    batch.cash.push_back({"task-144-rollback-cash", 1001, 1001, 880000, 0, "2026-05-30T12:00:00Z"});
    batch.positions.push_back({
        "task-144-rollback-position",
        1001,
        9999,
        std::nullopt,
        "",
        std::nullopt,
        "159509",
        "SYSTEM",
        1000000,
        120000,
        0,
        0,
        "2026-05-30T12:00:00Z",
    });

    etfdt::data_access::ShellAccountingSnapshotWriteRepository repository(*connection);
    const auto result = repository.writeSnapshotBatch(batch);

    bool ok = expect(!result, "invalid portfolio causes write batch failure");
    ok &= expect(countRows(*connection, "cash_snapshot") == 0, "cash write rolled back");
    ok &= expect(countRows(*connection, "position_snapshot") == 0, "position write rolled back");
    return ok ? 0 : 1;
}
