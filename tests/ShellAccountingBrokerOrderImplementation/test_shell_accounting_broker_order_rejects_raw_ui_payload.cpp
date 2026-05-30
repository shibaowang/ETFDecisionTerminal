#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    (void)createDraftAndConfirm(connection);
    auto payload = authorizedBrokerOrderDryRunPayload(latestTradeDraftId(connection));
    payload.insert(payload.size() - 1U, R"(,"rawOrderPayload":{"side":"BUY"})");
    const auto call = invokeBrokerOrderDryRun(connection, payload);
    return expect(!call.response.success, "raw UI order payload is rejected") ? 0 : 1;
}
