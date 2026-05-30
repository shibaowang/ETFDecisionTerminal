#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs98Text(sourceRoot(argc, argv)) + docs99Text(sourceRoot(argc, argv));
    return containsAllTokens(text,
               {"DataService-only",
                "future TradeDraft only through DataService",
                "no QML / Shell / Presenter direct draft write",
                "no generic write escape hatch",
                "authorization required"})
        ? 0
        : 1;
}
