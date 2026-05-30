#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "cash.summary");
    bool ok = expectReplayPayload(payload, "cash.summary", "cash replay");
    ok &= expect(payload.value("accountCashSummaries").toArray().size() == 1, "one replay cash summary");
    return ok ? 0 : 1;
}
