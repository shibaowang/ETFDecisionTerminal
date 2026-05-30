#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    const auto call = createConfirmedTradeAndDryRun(connection);
    bool ok = expectSuccessfulBrokerOrderDryRun(call);
    ok &= expect(call.payload.value("brokerOrderId").isNull(), "no broker order id");
    return ok ? 0 : 1;
}
