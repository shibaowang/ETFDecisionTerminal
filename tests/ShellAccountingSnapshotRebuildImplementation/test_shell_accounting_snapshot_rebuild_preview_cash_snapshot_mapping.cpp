#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "cash.summary");
    const auto preview = payload.value("snapshotRebuildPreview").toObject();
    const auto cashRows = preview.value("cashSnapshots").toArray();

    bool ok = expectReplayPayload(payload, "cash.summary", "cash preview");
    ok &= expect(!cashRows.isEmpty(), "cash preview rows");
    ok &= expect(cashRows.first().toObject().value("cashBalanceText").toString().toStdString() == "8800.00 CNY",
                 "cash preview balance");
    return ok ? 0 : 1;
}
