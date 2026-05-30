#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto create = invokeTradeDraftCreate(*connection);
    const auto logBefore = countRows(*connection, "trade_log");
    (void)connection->executeStatement("UPDATE trade_draft SET status = 'CANCELED' WHERE id = ?;",
                                      {etfdt::data_access::SqlStatementParameter::int64ValueOf(latestTradeDraftId(*connection))});
    const auto canceled = invokeTradeDraftConfirm(*connection, authorizedConfirmationPayload(latestTradeDraftId(*connection)));
    bool ok = expect(create.response.success, "draft fixture created");
    ok &= expect(!canceled.response.success, "canceled draft rejected");
    ok &= expect(countRows(*connection, "trade_log") == logBefore, "canceled draft writes no trade_log");
    return ok ? 0 : 1;
}

