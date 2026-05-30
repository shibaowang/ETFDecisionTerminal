#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedUsdReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    bool ok = expectReplayPayload(payload, "position.list", "missing FX replay");
    ok &= expect(hasIssue(payload, "FX_RATE_MISSING"), "missing FX issue is mapped");
    return ok ? 0 : 1;
}
