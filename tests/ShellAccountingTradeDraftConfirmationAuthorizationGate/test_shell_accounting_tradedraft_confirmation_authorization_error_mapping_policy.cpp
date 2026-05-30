#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs101Text(sourceRoot(argc, argv)) + "\n" + docs102Text(sourceRoot(argc, argv));
    return containsAllTokens(text, {
        "authorization missing",
        "confirmation disabled",
        "draft missing",
        "draft already confirmed",
        "draft canceled",
        "stale draft",
        "invalid draft payload",
        "ledger write authorization missing",
        "audit unavailable",
        "duplicate confirmation",
        "transaction rollback",
        "no silent success",
    }) ? 0 : 1;
}
