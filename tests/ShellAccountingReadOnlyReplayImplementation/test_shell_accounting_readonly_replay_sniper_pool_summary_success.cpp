#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "sniper_pool.summary");
    bool ok = expectReplayPayload(payload, "sniper_pool.summary", "sniper replay");
    ok &= expect(payload.value("tierSummary").toArray().size() == 1, "sniper pool facts remain available");
    return ok ? 0 : 1;
}
