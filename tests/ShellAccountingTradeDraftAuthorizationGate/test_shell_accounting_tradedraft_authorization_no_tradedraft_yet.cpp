#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto production = dataServiceBoundaryText(root);
    return containsAnyToken(production, draftImplementationTokens())
            || containsAnyToken(production, draftWriteTokens())
            || containsAnyToken(production, tradeExecutionTokens())
        ? 1
        : 0;
}
