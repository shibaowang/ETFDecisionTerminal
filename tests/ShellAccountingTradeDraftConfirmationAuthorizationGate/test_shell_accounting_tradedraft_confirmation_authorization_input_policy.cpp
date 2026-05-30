#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs101Text(sourceRoot(argc, argv)) + "\n" + docs102Text(sourceRoot(argc, argv));
    return containsAllTokens(text, {
        "existing unconfirmed TradeDraft",
        "draft status validation",
        "account / portfolio / instrument / side / quantity validation",
        "no UI raw payload",
        "no fake facts",
    }) ? 0 : 1;
}
