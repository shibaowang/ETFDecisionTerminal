#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    const auto first = createConfirmedTradeAndDryRun(connection);
    const auto afterFirst = countRows(connection, "audit_log");
    const auto second = invokeBrokerOrderDryRun(connection, authorizedBrokerOrderDryRunPayload(latestTradeDraftId(connection)));
    bool ok = expectSuccessfulBrokerOrderDryRun(first);
    ok &= expectDuplicateBrokerOrderDryRun(second);
    ok &= expect(countRows(connection, "audit_log") == afterFirst, "duplicate dry-run does not add audit row");
    return ok ? 0 : 1;
}
