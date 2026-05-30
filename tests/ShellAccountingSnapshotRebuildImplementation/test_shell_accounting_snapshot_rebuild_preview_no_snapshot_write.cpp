#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "cash.summary");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();

    bool ok = expectReplayPayload(payload, "cash.summary", "no snapshot write preview");
    ok &= expect(!payload.value("snapshotRebuilt").toBool(true), "snapshotRebuilt remains false");
    ok &= expect(!preview.value("snapshotWritten").toBool(true), "snapshotWritten false");
    ok &= expect(!preview.value("databaseWritten").toBool(true), "databaseWritten false");
    return ok ? 0 : 1;
}
