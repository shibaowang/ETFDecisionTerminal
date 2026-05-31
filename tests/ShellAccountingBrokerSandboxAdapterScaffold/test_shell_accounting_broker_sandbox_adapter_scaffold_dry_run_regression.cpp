#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"
#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"
#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main()
{
    auto& disabledProvider = defaultShellAccountingBrokerOrderPort();
    const auto disabled = disabledProvider.submitOrderEnvelope({});
    ShellAccountingBrokerSandboxOrderPortScaffold scaffold;
    const auto sandbox = scaffold.submitOrderEnvelope({});
    return expect(disabled.dryRunOnly, "TASK-159 disabled provider must remain dry-run-only") &&
            expect(disabled.disabled, "TASK-159 disabled provider must remain disabled") &&
            expect(sandbox.dryRunOnly, "TASK-161 scaffold must remain dry-run-only") &&
            expect(sandbox.disabled, "TASK-161 scaffold must remain disabled")
        ? 0
        : 1;
}
