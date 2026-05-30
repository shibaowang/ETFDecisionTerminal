#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto creation = tradeDraftCreationBoundaryText(root);
    const auto shell = shellBoundaryText(root);
    const auto confirmation = tradeDraftConfirmationBoundaryText(root);
    bool ok = !containsAnyToken(creation + shell, tradeExecutionTokens());
    ok &= containsAllTokens(
        confirmation,
        {"INSERT INTO trade_execution_group", "INSERT INTO trade_log", "TASK-150_TRADEDRAFT_CONFIRM"});
    ok &= !containsAnyToken(confirmation, {"brokerOrder(", "placeOrder", "executeOrder", "StrategyEngine::execute"});
    return ok ? 0 : 1;
}
