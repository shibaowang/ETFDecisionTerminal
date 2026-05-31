#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    return expect(!containsAnyToken(selectorSourceText(sourceRoot(argc, argv)), forbiddenWriteTableTokens()),
                  "selector must not write audit, ledger, execution group, or draft tables")
        ? 0
        : 1;
}
