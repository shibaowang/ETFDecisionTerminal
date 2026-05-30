#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    return containsAllTokens(
               productionQmlText(sourceRoot(argc, argv)),
               {
                   "objectName: \"shellAccountingTradingUiSection\"",
                   "objectName: \"shellAccountingCreateDraftButton\"",
                   "objectName: \"shellAccountingDraftReviewPanel\"",
                   "objectName: \"shellAccountingConfirmDraftButton\"",
               })
        ? 0
        : 1;
}
