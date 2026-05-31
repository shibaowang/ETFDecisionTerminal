#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main()
{
    const auto response = shellAccountingBrokerOrderPortForMode("sandbox").submitOrderEnvelope({});
    return expect(!response.success, "sandbox mode must not report success") &&
            expect(response.disabled, "sandbox mode remains disabled") &&
            expect(response.dryRunOnly, "sandbox mode remains dry-run-only")
        ? 0
        : 1;
}
