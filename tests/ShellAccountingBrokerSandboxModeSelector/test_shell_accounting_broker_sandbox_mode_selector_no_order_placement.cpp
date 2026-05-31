#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    return expect(!containsAnyToken(selectorSourceText(sourceRoot(argc, argv)), realOrderPlacementTokens()),
                  "selector must not place or submit orders")
        ? 0
        : 1;
}
