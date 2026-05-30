#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = createDraftAndConfirm(*connection);
    bool ok = expectSuccessfulConfirmation(call);
    ok &= expect(latestTradeExecutionGroupId(*connection) == latestTradeLogExecutionGroupId(*connection),
                 "trade_log references written execution group");
    return ok ? 0 : 1;
}

