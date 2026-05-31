#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(task160DocsText(root), {
        "TASK-160",
        "TASK-159",
        "future ShellAccounting sandbox broker adapter",
        "does not implement a sandbox adapter",
        "Future Implementation PR Requirements",
    }) ? 0 : 1;
}
