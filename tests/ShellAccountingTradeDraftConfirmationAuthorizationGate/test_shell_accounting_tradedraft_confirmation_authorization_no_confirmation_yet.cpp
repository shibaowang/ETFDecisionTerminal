#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    return containsAnyToken(dataServiceBoundaryText(sourceRoot(argc, argv)), confirmationImplementationTokens()) ? 1 : 0;
}
