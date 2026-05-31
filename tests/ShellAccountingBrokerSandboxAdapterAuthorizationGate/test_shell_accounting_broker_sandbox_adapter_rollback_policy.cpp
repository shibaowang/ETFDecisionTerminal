#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(task160DocsText(sourceRoot(argc, argv)), {
        "Rollback / Kill Switch / Fail-Closed Policy",
        "fall back to the TASK-159 disabled/null provider",
        "must not fake broker success",
        "or retry order placement",
    }) ? 0 : 1;
}
