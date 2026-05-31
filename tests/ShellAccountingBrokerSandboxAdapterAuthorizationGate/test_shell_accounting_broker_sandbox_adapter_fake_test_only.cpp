#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(task160DocsText(root), {"Fake Adapter Test-Only Policy", "fake broker adapter remains test-only"})
        && !containsAnyToken(productionText(root), {"TestOnlyFakeShellAccountingBrokerOrderPort"})
        ? 0
        : 1;
}
