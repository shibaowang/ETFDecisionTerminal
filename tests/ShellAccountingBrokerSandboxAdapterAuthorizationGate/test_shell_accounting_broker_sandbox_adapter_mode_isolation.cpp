#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(task160DocsText(sourceRoot(argc, argv)), {
        "Sandbox / Paper / Real Mode Isolation",
        "Sandbox, paper, and real modes must be isolated",
        "separate authorization boundary",
        "must not share credentials, endpoints, order ids, or side effects",
    }) ? 0 : 1;
}
