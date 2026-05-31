#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    return expect(!containsAnyToken(selectorSourceText(sourceRoot(argc, argv)), reconciliationCancellationTokens()),
                  "selector must not add reconciliation, cancellation, or correction flows")
        ? 0
        : 1;
}
