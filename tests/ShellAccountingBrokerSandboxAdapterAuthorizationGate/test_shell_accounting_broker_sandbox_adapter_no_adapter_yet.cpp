#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(docs119Text(root), {"Sandbox Adapter Not Implemented Yet"})
        && !containsAnyToken(productionText(root), concreteAdapterTokens())
        ? 0
        : 1;
}
