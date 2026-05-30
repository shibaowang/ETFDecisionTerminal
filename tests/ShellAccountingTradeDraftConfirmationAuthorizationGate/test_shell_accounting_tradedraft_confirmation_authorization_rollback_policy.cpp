#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs101Text(sourceRoot(argc, argv)) + "\n" + docs102Text(sourceRoot(argc, argv));
    return containsAllTokens(text, {
        "disable confirmation",
        "fallback keeps TradeDraft creation / audit / snapshot features unaffected",
        "no fake confirmation",
        "no broker fallback",
        "no strategy fallback",
    }) ? 0 : 1;
}
