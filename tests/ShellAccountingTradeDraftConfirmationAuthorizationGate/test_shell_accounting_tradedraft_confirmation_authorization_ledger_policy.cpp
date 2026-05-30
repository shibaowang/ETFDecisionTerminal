#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs101Text(sourceRoot(argc, argv)) + "\n" + docs102Text(sourceRoot(argc, argv));
    return containsAllTokens(text, {
        "trade_log is the factual ledger",
        "trade_execution_group and trade_log",
        "atomic",
        "no partial ledger write",
        "no duplicate confirmation",
    }) ? 0 : 1;
}
