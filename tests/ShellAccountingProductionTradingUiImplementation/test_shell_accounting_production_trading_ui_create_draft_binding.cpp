#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(productionQmlText(root), {".createDraft("})
            && containsAllTokens(
                shellServicesText(root),
                {"createDraft(", "accounting.tradedraft.create", "TASK-148_TRADEDRAFT_WRITE"})
        ? 0
        : 1;
}
