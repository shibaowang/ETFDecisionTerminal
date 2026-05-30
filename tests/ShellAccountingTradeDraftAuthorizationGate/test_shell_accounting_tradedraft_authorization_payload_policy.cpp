#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs98Text(sourceRoot(argc, argv)) + docs99Text(sourceRoot(argc, argv));
    return containsAllTokens(text,
               {"sanitized",
                "account",
                "portfolio",
                "instrument",
                "side",
                "quantity",
                "reason",
                "source snapshot id",
                "source replay id",
                "risk flags",
                "no raw SQL",
                "no full snapshot payload",
                "no full trade_log payload",
                "no stack trace"})
        ? 0
        : 1;
}
