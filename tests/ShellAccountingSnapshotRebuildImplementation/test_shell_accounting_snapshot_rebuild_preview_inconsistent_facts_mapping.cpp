#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedSellExceedsFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();

    bool ok = expectReplayPayload(payload, "position.list", "inconsistent preview");
    ok &= expect(hasIssue(payload, "SELL_EXCEEDS_POSITION"), "inconsistent facts issue preserved");
    ok &= expect(preview.value("status").toString().toStdString() == "ISSUE", "preview issue status");
    ok &= expect(!preview.value("databaseWritten").toBool(true), "inconsistent preview no DB write");
    return ok ? 0 : 1;
}
