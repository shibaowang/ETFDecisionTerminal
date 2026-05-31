#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main()
{
    const auto response = shellAccountingBrokerOrderPortForMode("sandbox").submitOrderEnvelope({});
    return expect(shellAccountingBrokerOrderPortModeFromString("sandbox") == ShellAccountingBrokerOrderPortMode::Sandbox,
                  "sandbox mode must parse explicitly") &&
            expect(response.status == ShellAccountingBrokerOrderStatus::Error, "sandbox scaffold returns unavailable error status") &&
            expect(response.errorCode == "BROKER_SANDBOX_NOT_CONFIGURED", "sandbox mode returns scaffold not-configured error") &&
            expect(response.brokerMode == "sandbox", "sandbox mode reports sandbox mode")
        ? 0
        : 1;
}
