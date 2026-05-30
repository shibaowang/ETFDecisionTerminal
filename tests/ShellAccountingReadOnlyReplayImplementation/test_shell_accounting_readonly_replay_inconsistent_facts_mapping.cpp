#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedSellExceedsFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    bool ok = expectReplayPayload(payload, "position.list", "inconsistent replay");
    ok &= expect(hasIssue(payload, "SELL_EXCEEDS_POSITION"), "sell exceeds position issue is mapped");
    return ok ? 0 : 1;
}
