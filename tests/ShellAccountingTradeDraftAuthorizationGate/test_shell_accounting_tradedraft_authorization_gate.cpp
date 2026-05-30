#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = docs98Text(root);
    const auto plan = docs99Text(root);
    return containsAllTokens(gate + plan,
               {"TASK-119 through TASK-146",
                "TradeDraft implementation must be a separate",
                "TradeDraft is not trade execution",
                "TradeDraft is not strategy execution",
                "TradeDraft does not trigger broker order"})
        ? 0
        : 1;
}
