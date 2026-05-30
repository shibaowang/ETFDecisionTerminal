#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    return containsAllTokens(
               productionQmlText(sourceRoot(argc, argv)),
               {
                   "objectName: \"shellAccountingConfirmReviewCheckBox\"",
                   "confirmationCheck.checked",
                   "I reviewed this draft",
                   "ledgerStatus !== \"CONFIRMED_LEDGER\"",
               })
        ? 0
        : 1;
}
