#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto missingSource = invokeSnapshotWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","calculationMode":"readonlyReplay","authorization":"TASK-144_SNAPSHOT_WRITE"})");
    const auto missingReplay = invokeSnapshotWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","source":"snapshotRebuildPreview","authorization":"TASK-144_SNAPSHOT_WRITE"})");
    const auto missingAuth = invokeSnapshotWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","calculationMode":"readonlyReplay","source":"snapshotRebuildPreview"})");

    bool ok = expect(!missingSource.response.success, "missing source is rejected");
    ok &= expect(!missingReplay.response.success, "missing readonlyReplay mode is rejected");
    ok &= expect(!missingAuth.response.success, "missing authorization is rejected");
    ok &= expect(countRows(*connection, "cash_snapshot") == 0, "no cash snapshot written");
    ok &= expect(countRows(*connection, "position_snapshot") == 0, "no position snapshot written");
    ok &= expect(countRows(*connection, "portfolio_summary") == 0, "no portfolio summary written");
    return ok ? 0 : 1;
}
