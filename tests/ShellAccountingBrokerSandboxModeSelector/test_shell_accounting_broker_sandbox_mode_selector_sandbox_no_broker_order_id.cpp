#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto response = shellAccountingBrokerOrderPortForMode("sandbox").submitOrderEnvelope({});
    return expect(response.sanitizedCorrelationId.empty(), "sandbox mode must not return broker order id") &&
            expect(!containsAnyToken(selectorSourceText(sourceRoot(argc, argv)), realOrderIdTokens()),
                   "selector source must not contain real broker order id tokens")
        ? 0
        : 1;
}
