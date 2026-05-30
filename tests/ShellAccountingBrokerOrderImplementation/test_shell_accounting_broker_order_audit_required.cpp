#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    const auto call = createConfirmedTradeAndDryRun(connection);
    bool ok = expectSuccessfulBrokerOrderDryRun(call);
    ok &= expect(!latestBrokerOrderAuditPayload(connection).empty(), "broker dry-run audit exists");
    ok &= expect(latestBrokerOrderAuditPayload(connection).find("accounting.broker_order.dry_run") != std::string::npos,
                 "dry-run audit payload records source action");
    return ok ? 0 : 1;
}
