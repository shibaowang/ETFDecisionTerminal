#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    (void)createDraftAndConfirm(connection);
    const auto before = countRows(connection, "trade_draft");
    const auto call = invokeBrokerOrderDryRun(connection, authorizedBrokerOrderDryRunPayload(latestTradeDraftId(connection)));
    return expectSuccessfulBrokerOrderDryRun(call)
            && expect(countRows(connection, "trade_draft") == before, "no trade_draft write")
        ? 0
        : 1;
}
