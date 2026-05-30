#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto shell = shellBoundaryText(sourceRoot(argc, argv));
    return containsAnyToken(shell, uiDraftTriggerTokens()) ? 1 : 0;
}
