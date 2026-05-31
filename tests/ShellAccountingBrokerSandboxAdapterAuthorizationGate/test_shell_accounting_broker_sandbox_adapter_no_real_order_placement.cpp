#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(task160DocsText(root), {"No Real Order Placement Policy", "No Real Order Placement"})
        && !containsAnyToken(productionText(root), realOrderPlacementTokens())
        ? 0
        : 1;
}
