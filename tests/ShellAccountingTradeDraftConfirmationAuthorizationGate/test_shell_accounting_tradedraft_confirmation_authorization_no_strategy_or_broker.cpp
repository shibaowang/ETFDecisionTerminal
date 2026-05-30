#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAnyToken(shellBoundaryText(root) + "\n" + dataServiceBoundaryText(root), strategyOrBrokerTokens()) ? 1 : 0;
}
