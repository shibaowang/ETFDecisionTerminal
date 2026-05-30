#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs101Text(sourceRoot(argc, argv)) + "\n" + docs102Text(sourceRoot(argc, argv));
    return containsAllTokens(text, {
        "future confirmation only through DataService",
        "no QML / Shell / Presenter direct ledger write",
        "no generic write escape hatch",
        "explicit authorization required",
    }) ? 0 : 1;
}
