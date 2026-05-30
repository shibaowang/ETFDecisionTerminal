#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    return containsAnyToken(shellBoundaryText(sourceRoot(argc, argv)), shellDraftTriggerTokens()) ? 1 : 0;
}
