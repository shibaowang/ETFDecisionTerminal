#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();

    bool ok = expectReplayPayload(payload, "position.list", "preview replay input");
    ok &= expect(preview.value("source").toString().toStdString() == "TASK-139 read-only replay", "preview source");
    ok &= expect(preview.value("input").toString().toStdString() == "readonlyReplay", "preview input");
    ok &= expect(payload.value("replayExecuted").toBool(false), "replay executed before preview");
    return ok ? 0 : 1;
}
