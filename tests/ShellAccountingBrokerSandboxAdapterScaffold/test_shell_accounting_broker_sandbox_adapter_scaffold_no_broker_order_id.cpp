#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"
#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    ShellAccountingBrokerSandboxOrderPortScaffold scaffold;
    const auto response = scaffold.submitOrderEnvelope({});
    const auto source = scaffoldSourceText(sourceRoot(argc, argv));
    return expect(response.sanitizedCorrelationId.empty(), "sandbox scaffold must not return broker order id") &&
            expect(!containsAnyToken(source, realOrderIdTokens()), "sandbox scaffold source must not store real broker order ids")
        ? 0
        : 1;
}
