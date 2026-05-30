#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(shellBoundaryText(root), authorizedDraftConfirmBindingTokens()) &&
            productionQmlText(root).find("accounting.tradedraft.confirm") == std::string::npos &&
            productionQmlText(root).find("TASK-150_TRADEDRAFT_CONFIRM") == std::string::npos
        ? 0
        : 1;
}
