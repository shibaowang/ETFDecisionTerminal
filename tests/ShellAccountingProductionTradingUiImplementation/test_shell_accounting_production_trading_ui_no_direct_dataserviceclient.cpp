#include "ShellAccountingProductionTradingUiImplementation.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto page = readTextFile(
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");
    return containsAnyToken(page, directDataServiceClientTokens()) ? 1 : 0;
}
