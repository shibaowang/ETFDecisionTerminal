#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(task160DocsText(root), {"No Reconciliation Policy", "No Reconciliation"})
        && !containsAnyToken(productionText(root), reconciliationTokens())
        ? 0
        : 1;
}
