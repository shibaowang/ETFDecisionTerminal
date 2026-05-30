#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto call = invokeTradeDraftCreate(*connection);
    bool ok = expectSuccessfulTradeDraft(call);
    ok &= expect(countRows(*connection, "trade_draft") == 1, "one trade_draft row written");
    ok &= expect(countRows(*connection, "audit_log") == 1, "one audit_log row written");
    ok &= expect(countRows(*connection, "trade_log") == 1, "fixture trade_log unchanged");
    ok &= expect(countRows(*connection, "trade_execution_group") == 1,
                 "fixture trade_execution_group unchanged");
    return ok ? 0 : 1;
}
