#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    return expectSuccessfulBrokerOrderDryRun(createConfirmedTradeAndDryRun(connection)) ? 0 : 1;
}
