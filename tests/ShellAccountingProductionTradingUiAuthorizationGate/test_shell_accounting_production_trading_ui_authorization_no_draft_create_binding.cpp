#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(shellBoundaryText(root), authorizedDraftCreateBindingTokens()) &&
            productionQmlText(root).find("accounting.tradedraft.create") == std::string::npos &&
            productionQmlText(root).find("TASK-148_TRADEDRAFT_WRITE") == std::string::npos
        ? 0
        : 1;
}
