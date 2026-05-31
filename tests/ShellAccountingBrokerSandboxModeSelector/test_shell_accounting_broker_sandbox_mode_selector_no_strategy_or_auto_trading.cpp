#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto source = selectorSourceText(sourceRoot(argc, argv));
    return expect(!containsAnyToken(source, strategyExecutionTokens()), "selector must not execute strategy") &&
            expect(!containsAnyToken(source, automaticTradingTokens()), "selector must not enable automatic trading")
        ? 0
        : 1;
}
