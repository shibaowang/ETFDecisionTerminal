#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    (void)createDraftAndConfirm(connection);
    (void)connection->executeStatement("DROP TABLE audit_log;");
    const auto call = invokeBrokerOrderDryRun(
        connection,
        authorizedBrokerOrderDryRunPayload(latestTradeDraftId(connection)));
    return expect(!call.response.success, "audit failure blocks success") ? 0 : 1;
}
