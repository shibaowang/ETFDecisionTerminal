#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = docs101Text(root);
    const auto plan = docs102Text(root);
    return containsAllTokens(gate + plan, {
        "TASK-119",
        "TASK-148",
        "future confirmation implementation must be a separate",
        "Confirmation is not broker order",
        "Confirmation is not automatic trading",
        "Confirmation is not strategy execution",
    }) ? 0 : 1;
}
