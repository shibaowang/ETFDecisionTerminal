#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main()
{
    const auto defaultResponse = defaultShellAccountingBrokerOrderPort().submitOrderEnvelope({});
    const auto selectorResponse =
        shellAccountingBrokerOrderPortForMode(defaultShellAccountingBrokerOrderPortMode()).submitOrderEnvelope({});
    return expect(defaultResponse.errorCode == "BROKER_ORDER_DISABLED", "default provider remains disabled/null") &&
            expect(selectorResponse.errorCode == "BROKER_ORDER_DISABLED",
                   "selector default mode resolves to disabled/null provider")
        ? 0
        : 1;
}
