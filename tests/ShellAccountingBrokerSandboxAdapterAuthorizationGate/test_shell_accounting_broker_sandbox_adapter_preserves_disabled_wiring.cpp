#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(dataServiceApiText(root), {
        "ShellAccountingBrokerOrderPortProvider.h",
        "defaultShellAccountingBrokerOrderPort",
        "DisabledShellAccountingBrokerOrderPort",
    }) && containsAllTokens(readTextFile(root / "docs" / "117_shell_accounting_broker_adapter_disabled_wiring.md"), {
        "TASK-159",
        "disabled/null broker adapter",
    }) ? 0 : 1;
}
