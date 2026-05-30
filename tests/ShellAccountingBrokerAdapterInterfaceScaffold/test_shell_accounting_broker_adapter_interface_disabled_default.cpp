#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int, char**)
{
    etfdt::dataservice::DisabledShellAccountingBrokerOrderPort port;
    const auto response = port.submitOrderEnvelope(validRequest());
    bool ok = expect(!response.success, "disabled default is not success");
    ok &= expect(response.disabled, "disabled default marks disabled");
    ok &= expect(response.dryRunOnly, "disabled default remains dry-run-only");
    ok &= expect(response.errorCode == "BROKER_ORDER_DISABLED", "disabled default maps error code");
    ok &= expect(response.sanitizedCorrelationId.empty(), "disabled default has no order id");
    return ok ? 0 : 1;
}
