#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto groupBefore = countRows(*connection, "trade_execution_group");
    const auto logBefore = countRows(*connection, "trade_log");
    const auto auditBefore = countRows(*connection, "audit_log");
    const auto call = createDraftAndConfirm(*connection);
    bool ok = expectSuccessfulConfirmation(call);
    ok &= expect(countRows(*connection, "trade_execution_group") == groupBefore + 1,
                 "one execution group written");
    ok &= expect(countRows(*connection, "trade_log") == logBefore + 1, "one trade_log row written");
    ok &= expect(countRows(*connection, "audit_log") == auditBefore + 2,
                 "create and confirmation audits written");
    return ok ? 0 : 1;
}

