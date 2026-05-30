#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs98Text(sourceRoot(argc, argv)) + docs99Text(sourceRoot(argc, argv));
    return containsAllTokens(text,
               {"authorization missing",
                "disabled",
                "input unavailable",
                "stale snapshot",
                "replay unavailable",
                "invalid instrument",
                "invalid side",
                "invalid quantity",
                "risk check failed",
                "audit unavailable",
                "duplicate draft",
                "rollback",
                "no silent success"})
        ? 0
        : 1;
}
