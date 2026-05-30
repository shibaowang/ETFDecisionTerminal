#include "ShellAccountingBrokerAdapterDisabledWiring.h"

#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    const auto response = etfdt::dataservice::defaultShellAccountingBrokerOrderPort().submitOrderEnvelope({});
    bool ok = true;
    ok &= expect(response.status == etfdt::dataservice::ShellAccountingBrokerOrderStatus::Disabled,
                 "default port returns disabled status");
    ok &= expect(response.disabled, "default port is disabled");
    ok &= expect(!response.success, "default port does not succeed");
    ok &= expect(containsAllTokens(dataServiceApiText(sourceRoot(argc, argv)), {
                     "defaultShellAccountingBrokerOrderPort",
                     "DisabledShellAccountingBrokerOrderPort",
                     "brokerPortStatus",
                 }),
                 "DataServiceApi wires disabled port");
    return ok ? 0 : 1;
}
