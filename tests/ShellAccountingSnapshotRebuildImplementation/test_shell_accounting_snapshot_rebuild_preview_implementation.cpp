#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonObject>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();

    bool ok = expectReplayPayload(payload, "position.list", "snapshot rebuild preview");
    ok &= expect(!preview.isEmpty(), "preview payload exists");
    ok &= expect(preview.value("implemented").toBool(false), "preview implemented");
    ok &= expect(preview.value("previewOnly").toBool(false), "preview only");
    ok &= expect(!preview.value("databaseWritten").toBool(true), "preview does not write DB");
    ok &= expect(!preview.value("snapshotWritten").toBool(true), "preview does not write snapshot");
    ok &= expect(docs86Text(sourceRoot(argc, argv)).find("TASK-140") != std::string::npos, "prior docs remain available");
    return ok ? 0 : 1;
}
