#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"
#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main()
{
    ShellAccountingBrokerSandboxOrderPortScaffold scaffold;
    const auto response = scaffold.submitOrderEnvelope({});
    return expect(response.status == ShellAccountingBrokerOrderStatus::Error, "sandbox scaffold returns explicit error status") &&
            expect(response.disabled, "sandbox scaffold is disabled") &&
            expect(response.dryRunOnly, "sandbox scaffold is dry-run only") &&
            expect(response.brokerMode == "sandbox", "sandbox scaffold reports sandbox mode") &&
            expect(response.sanitizedMessage == "Sandbox broker adapter is not configured.",
                   "sandbox scaffold returns sanitized unavailable message")
        ? 0
        : 1;
}
