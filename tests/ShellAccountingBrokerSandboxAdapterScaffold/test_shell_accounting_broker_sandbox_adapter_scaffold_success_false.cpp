#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"
#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main()
{
    ShellAccountingBrokerSandboxOrderPortScaffold scaffold;
    ShellAccountingBrokerOrderRequest request;
    request.idempotencyKey = "test-idempotency";
    const auto response = scaffold.submitOrderEnvelope(request);
    return expect(!response.success, "sandbox scaffold must never report success") &&
            expect(response.disabled, "sandbox scaffold remains fail-closed with an idempotency key") &&
            expect(response.sanitizedMessage.find("requested envelope") != std::string::npos,
                   "sandbox scaffold keeps response sanitized for keyed requests")
        ? 0
        : 1;
}
