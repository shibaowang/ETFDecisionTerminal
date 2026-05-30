#include "ShellAccountingBrokerAdapterDisabledWiring.h"

#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main()
{
    const auto response = etfdt::dataservice::defaultShellAccountingBrokerOrderPort().submitOrderEnvelope({});
    bool ok = true;
    ok &= expect(response.brokerMode == "disabled", "broker mode is disabled");
    ok &= expect(response.errorCode == "BROKER_ORDER_DISABLED", "disabled error code is explicit");
    ok &= expect(response.sanitizedCorrelationId.empty(), "no real correlation or order id");
    return ok ? 0 : 1;
}
