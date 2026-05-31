#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto checkedText = scaffoldSourceText(root) + "\n" + task161DocsText(root);
    return expect(!containsAnyToken(checkedText, credentialValueTokens()),
                  "TASK-161 source and docs must not contain usable endpoint or secret values")
        ? 0
        : 1;
}
