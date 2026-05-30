#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = docs104Text(root);
    const auto plan = docs105Text(root);
    return containsAllTokens(gate + plan, {
        "TASK-119",
        "TASK-150",
        "Future production trading UI implementation must be a separate",
        "Trading UI is not broker order placement",
        "Trading UI is not automatic trading",
    }) ? 0 : 1;
}
