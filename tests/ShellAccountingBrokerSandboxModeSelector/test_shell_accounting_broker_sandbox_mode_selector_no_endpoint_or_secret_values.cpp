#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto checkedText = selectorSourceText(root) + "\n" + task162DocsText(root);
    return expect(!containsAnyToken(checkedText, credentialValueTokens()),
                  "TASK-162 source and docs must not contain usable endpoint or secret values")
        ? 0
        : 1;
}
