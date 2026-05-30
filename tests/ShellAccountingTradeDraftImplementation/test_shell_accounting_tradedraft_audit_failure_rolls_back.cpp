#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    auto trigger = connection->executeSql(
        "CREATE TRIGGER task_148_abort_audit BEFORE INSERT ON audit_log "
        "BEGIN SELECT RAISE(ABORT, 'task-148 audit failure'); END;");
    bool ok = expect(trigger.hasValue(), "audit failure trigger created");

    const auto call = invokeTradeDraftCreate(*connection);
    ok &= expect(!call.response.success, "audit failure rejects TradeDraft action");
    ok &= expect(countRows(*connection, "trade_draft") == 0, "audit failure rolls back draft");
    ok &= expect(countRows(*connection, "audit_log") == 0, "audit failure leaves no audit row");
    return ok ? 0 : 1;
}
