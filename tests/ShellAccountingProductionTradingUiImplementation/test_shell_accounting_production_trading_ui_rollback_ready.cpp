#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    return containsAllTokens(
               docs106Text(sourceRoot(argc, argv)),
               {"disable production trading UI", "fallback to the read-only accounting page", "no fake draft", "no fake confirmation"})
        ? 0
        : 1;
}
