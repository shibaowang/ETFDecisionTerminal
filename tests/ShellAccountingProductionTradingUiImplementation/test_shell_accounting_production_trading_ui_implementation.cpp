#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(docs106Text(root), {"TASK-152", "production trading UI"})
            && containsAllTokens(
                productionQmlText(root),
                {"objectName: \"shellAccountingTradingUiSection\"", ".createDraft(", ".confirmDraft("})
        ? 0
        : 1;
}
