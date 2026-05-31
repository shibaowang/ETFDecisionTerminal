#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main()
{
    return expect(defaultShellAccountingBrokerOrderPortMode() == ShellAccountingBrokerOrderPortMode::Disabled,
                  "default broker mode must be disabled")
        ? 0
        : 1;
}
