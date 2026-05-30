#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    const auto payload = invokeReplayAction(*connection, "position.list");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();

    bool ok = expectReplayPayload(payload, "position.list", "empty preview");
    ok &= expect(preview.value("status").toString().toStdString() == "READY", "empty replay preview is explicit");
    ok &= expect(preview.value("positionSnapshots").toArray().isEmpty(), "empty input does not fake preview rows");
    ok &= expect(!preview.value("snapshotWritten").toBool(true), "empty input does not write snapshot");
    return ok ? 0 : 1;
}
