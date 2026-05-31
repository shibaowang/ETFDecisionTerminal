#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main()
{
    const auto empty = shellAccountingBrokerOrderPortForMode("").submitOrderEnvelope({});
    const auto unknown = shellAccountingBrokerOrderPortForMode("paper").submitOrderEnvelope({});
    return expect(shellAccountingBrokerOrderPortModeFromString("") == ShellAccountingBrokerOrderPortMode::Unsupported,
                  "empty mode must parse as unsupported") &&
            expect(shellAccountingBrokerOrderPortModeFromString("paper") == ShellAccountingBrokerOrderPortMode::Unsupported,
                   "unknown mode must parse as unsupported") &&
            expect(empty.errorCode == "BROKER_ORDER_DISABLED", "empty mode must fail closed to disabled provider") &&
            expect(unknown.errorCode == "BROKER_ORDER_DISABLED", "unknown mode must fail closed to disabled provider")
        ? 0
        : 1;
}
