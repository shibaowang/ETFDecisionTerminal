#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(productionQmlText(root), {".confirmDraft("})
            && containsAllTokens(
                shellServicesText(root),
                {"confirmDraft(", "accounting.tradedraft.confirm", "TASK-150_TRADEDRAFT_CONFIRM"})
        ? 0
        : 1;
}
