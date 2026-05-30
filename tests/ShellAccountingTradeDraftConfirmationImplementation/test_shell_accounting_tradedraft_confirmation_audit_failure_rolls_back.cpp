#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto create = invokeTradeDraftCreate(*connection);
    const auto groupBefore = countRows(*connection, "trade_execution_group");
    const auto logBefore = countRows(*connection, "trade_log");
    (void)connection->executeSql(
        "CREATE TRIGGER task150_block_audit BEFORE INSERT ON audit_log "
        "BEGIN SELECT RAISE(FAIL, 'task150 audit blocked'); END;");
    const auto failed = invokeTradeDraftConfirm(*connection, authorizedConfirmationPayload(latestTradeDraftId(*connection)));
    bool ok = expect(create.response.success, "draft fixture created");
    ok &= expect(!failed.response.success, "audit failure rejects confirmation");
    ok &= expect(countRows(*connection, "trade_execution_group") == groupBefore,
                 "audit failure rolls back group");
    ok &= expect(countRows(*connection, "trade_log") == logBefore, "audit failure rolls back trade_log");
    ok &= expect(latestTradeDraftStatus(*connection) == "ACTIVE", "audit failure rolls back draft status");
    return ok ? 0 : 1;
}

