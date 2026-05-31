#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = task160DocsText(root);
    return containsAllTokens(docs, {"Credentials Isolation Policy", "No Real Credential / Secret / Token / Key / Password Value"})
        && !containsAnyToken(productionText(root), credentialValueTokens())
        && !containsAnyToken(docs, credentialValueTokens())
        ? 0
        : 1;
}
