#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    return containsAllTokens(
               docs106Text(sourceRoot(argc, argv)),
               {
                   "authorization missing",
                   "trading UI disabled",
                   "draft creation failed",
                   "confirmation failed",
                   "transport unavailable",
                   "no silent success",
               })
        ? 0
        : 1;
}
