#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();
    const auto positions = preview.value("positionSnapshots").toArray();

    bool ok = expectReplayPayload(payload, "position.list", "position preview");
    ok &= expect(!positions.isEmpty(), "position preview rows");
    ok &= expect(positions.first().toObject().value("instrumentCode").toString().toStdString() == "159509",
                 "position preview instrument");
    ok &= expect(positions.first().toObject().value("quantityText").toString().toStdString() == "1",
                 "position preview quantity");
    return ok ? 0 : 1;
}
