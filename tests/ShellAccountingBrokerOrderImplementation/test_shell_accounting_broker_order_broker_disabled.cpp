#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    const auto call = createConfirmedTradeAndDryRun(connection);
    bool ok = expect(call.payload.value("brokerDisabled").toBool(false), "brokerDisabled true in dry-run");
    (void)createDraftAndConfirm(connection);
    auto disabledPayload = authorizedBrokerOrderDryRunPayload(latestTradeDraftId(connection));
    disabledPayload.insert(disabledPayload.size() - 1U, R"(,"brokerOrderDisabled":true)");
    ok &= expect(!invokeBrokerOrderDryRun(connection, disabledPayload).response.success,
                 "explicitly disabled dry-run fails");
    return ok ? 0 : 1;
}
