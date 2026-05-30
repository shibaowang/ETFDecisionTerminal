#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    const auto previewPrivacy = payload.value("snapshotRebuildPreview").toObject().value("privacy").toObject();

    bool ok = expectReplayPayload(payload, "position.list", "preview privacy");
    ok &= expect(!previewPrivacy.value("rawSqlExposed").toBool(true), "preview no raw SQL");
    ok &= expect(!previewPrivacy.value("rawTradeLogPayloadExposed").toBool(true), "preview no raw trade_log");
    ok &= expect(!previewPrivacy.value("rawReplayPayloadExposed").toBool(true), "preview no raw replay payload");
    ok &= expect(!previewPrivacy.value("internalStackExposed").toBool(true), "preview no stack");
    return ok ? 0 : 1;
}
