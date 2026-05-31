#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    return expect(!containsAnyToken(selectorSourceText(sourceRoot(argc, argv)), credentialReadTokens()),
                  "selector must not read credentials")
        ? 0
        : 1;
}
