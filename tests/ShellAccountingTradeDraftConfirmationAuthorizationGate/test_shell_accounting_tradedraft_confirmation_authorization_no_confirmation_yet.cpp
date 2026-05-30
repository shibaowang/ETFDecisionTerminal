#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = dataServiceBoundaryText(sourceRoot(argc, argv));
    const bool authorized = containsAllTokens(
        text,
        {"kActionAccountingTradeDraftConfirm",
         "accounting.tradedraft.confirm",
         "TASK-150_TRADEDRAFT_CONFIRM",
         "ShellAccountingTradeDraftConfirmationRepository",
         "handleAccountingTradeDraftConfirm"});
    const bool forbidden = containsAnyToken(text, strategyOrBrokerTokens());
    return authorized && !forbidden ? 0 : 1;
}
