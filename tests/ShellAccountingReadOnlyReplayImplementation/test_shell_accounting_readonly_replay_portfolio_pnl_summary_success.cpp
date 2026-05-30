#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "portfolio.pnl.summary");
    bool ok = expectReplayPayload(payload, "portfolio.pnl.summary", "portfolio replay");
    ok &= expect(payload.value("portfolioPnl").isObject(), "portfolio pnl object exists");
    ok &= expect(payload.value("status").toString().toStdString() == "WARNING", "missing price is explicit warning");
    return ok ? 0 : 1;
}
