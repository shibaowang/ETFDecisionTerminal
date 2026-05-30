#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    return containsAnyToken(productionQmlText(sourceRoot(argc, argv)), directDbTokens()) ? 1 : 0;
}
