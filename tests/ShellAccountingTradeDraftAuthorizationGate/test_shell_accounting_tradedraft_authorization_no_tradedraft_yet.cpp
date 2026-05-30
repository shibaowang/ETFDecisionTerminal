#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto production = dataServiceBoundaryText(root);
    const auto creation = tradeDraftCreationBoundaryText(root);
    bool ok = containsAllTokens(
        production,
        {"kActionAccountingTradeDraftCreate",
         "handleAccountingTradeDraftCreate",
         "ShellAccountingTradeDraftRepository",
         "TASK-148_TRADEDRAFT_WRITE",
         "INSERT INTO trade_draft",
         "INSERT INTO audit_log"});
    ok &= !containsAnyToken(
        creation,
        {"INSERT INTO trade_log",
         "UPDATE trade_log",
         "INSERT INTO trade_execution_group",
         "UPDATE trade_execution_group",
         "brokerOrder(",
         "placeOrder",
         "strategyExecute("});
    ok &= containsAllTokens(
        production,
        {"kActionAccountingTradeDraftConfirm",
         "handleAccountingTradeDraftConfirm",
         "ShellAccountingTradeDraftConfirmationRepository",
         "TASK-150_TRADEDRAFT_CONFIRM"});
    return ok ? 0 : 1;
}
