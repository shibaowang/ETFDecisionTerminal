#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs98Text(sourceRoot(argc, argv)) + docs99Text(sourceRoot(argc, argv));
    return containsAllTokens(text,
               {"disable TradeDraft",
                "snapshot/replay/audit features unaffected",
                "no fake draft",
                "no trade execution fallback",
                "no strategy fallback",
                "no broker fallback"})
        ? 0
        : 1;
}
