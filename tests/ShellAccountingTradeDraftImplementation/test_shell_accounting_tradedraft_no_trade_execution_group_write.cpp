#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto before = countRows(*connection, "trade_execution_group");

    const auto call = invokeTradeDraftCreate(*connection);
    bool ok = expectSuccessfulTradeDraft(call);
    ok &= expect(countRows(*connection, "trade_execution_group") == before,
                 "TradeDraft does not write trade_execution_group");
    return ok ? 0 : 1;
}
