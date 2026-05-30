#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto logBefore = countRows(*connection, "trade_log");
    const auto first = createDraftAndConfirm(*connection);
    const auto second = invokeTradeDraftConfirm(*connection, authorizedConfirmationPayload(latestTradeDraftId(*connection)));
    bool ok = expectSuccessfulConfirmation(first);
    ok &= expectDuplicateConfirmation(second);
    ok &= expect(countRows(*connection, "trade_log") == logBefore + 1,
                 "already confirmed draft writes no duplicate log");
    return ok ? 0 : 1;
}

