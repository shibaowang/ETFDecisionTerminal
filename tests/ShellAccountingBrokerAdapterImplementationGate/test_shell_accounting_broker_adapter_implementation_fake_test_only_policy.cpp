#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(docsText(root), {
        "fake test-only",
        "allowed only under tests",
        "never in production runtime",
    }) && !containsAnyToken(productionText(root), {"TestOnlyFakeShellAccountingBrokerOrderPort"}) ? 0 : 1;
}
