#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto text = dataServiceActionsText(root) + tradeDraftRepositoryText(root) + shellBoundaryText(root);
    return containsAnyToken(text, strategyTokens()) ? 1 : 0;
}
