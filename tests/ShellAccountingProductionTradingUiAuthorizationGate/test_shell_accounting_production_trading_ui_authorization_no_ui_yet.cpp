#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto qml = productionQmlText(sourceRoot(argc, argv));
    return containsAllTokens(qml, authorizedTradingUiTokens()) &&
            !containsAnyToken(qml, forbiddenTradingUiTokens())
        ? 0
        : 1;
}
