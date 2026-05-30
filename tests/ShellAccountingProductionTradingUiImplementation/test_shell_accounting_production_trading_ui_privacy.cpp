#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(
               docs106Text(root),
               {"no raw SQL", "no full trade_log payload", "no internal stack trace"})
            && !containsAnyToken(productionQmlText(root), privacyLeakTokens())
        ? 0
        : 1;
}
