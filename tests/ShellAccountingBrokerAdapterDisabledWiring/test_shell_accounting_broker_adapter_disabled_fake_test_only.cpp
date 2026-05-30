#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(docs117Text(root), {
        "Fake Adapter Test-Only Policy",
        "fake adapter remains test-only",
    }) && !containsAnyToken(dataServiceApiText(root), {"TestOnlyFakeShellAccountingBrokerOrderPort"}) ? 0 : 1;
}
