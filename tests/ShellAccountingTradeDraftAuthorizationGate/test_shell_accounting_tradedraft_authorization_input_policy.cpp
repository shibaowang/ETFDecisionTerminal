#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs98Text(sourceRoot(argc, argv)) + docs99Text(sourceRoot(argc, argv));
    return containsAllTokens(text,
               {"authorized snapshot",
                "replay",
                "accounting summary",
                "no raw UI payload",
                "no fake facts",
                "no unauthorized strategy output"})
        ? 0
        : 1;
}
