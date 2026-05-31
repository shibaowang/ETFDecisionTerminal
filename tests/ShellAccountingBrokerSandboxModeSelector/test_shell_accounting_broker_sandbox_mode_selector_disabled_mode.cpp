#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main()
{
    const auto response = shellAccountingBrokerOrderPortForMode("disabled").submitOrderEnvelope({});
    return expect(response.status == ShellAccountingBrokerOrderStatus::Disabled, "disabled mode returns disabled status") &&
            expect(response.disabled, "disabled mode remains disabled") &&
            expect(response.dryRunOnly, "disabled mode remains dry-run-only") &&
            expect(response.brokerMode == "disabled", "disabled mode uses disabled broker mode")
        ? 0
        : 1;
}
