#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    return containsAnyToken(shellBoundaryText(sourceRoot(argc, argv)), draftCreateBindingTokens()) ? 1 : 0;
}
