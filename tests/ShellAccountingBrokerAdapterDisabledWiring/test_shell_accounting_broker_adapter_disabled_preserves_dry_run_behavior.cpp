#include "ShellAccountingBrokerAdapterDisabledWiring.h"

#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    auto connection = etfdt::tests::shell_accounting_broker_order::openMigratedDatabase(migrationPath(argc, argv));
    etfdt::tests::shell_accounting_broker_order::seedReplayFixture(connection);
    const auto call = etfdt::tests::shell_accounting_broker_order::createConfirmedTradeAndDryRun(connection);

    bool ok = etfdt::tests::shell_accounting_broker_order::expectSuccessfulBrokerOrderDryRun(call);
    ok &= expect(call.payload.value("brokerPortMode").toString().toStdString() == "disabled",
                 "dry-run payload keeps disabled broker port mode");
    ok &= expect(call.payload.value("brokerPortStatus").toString().toStdString() == "DISABLED",
                 "dry-run payload reports disabled broker port status");
    ok &= expect(call.payload.value("brokerPortDisabled").toBool(false), "broker port disabled field");
    ok &= expect(call.payload.value("brokerPortDryRunOnly").toBool(false), "broker port dryRunOnly field");
    return ok ? 0 : 1;
}
