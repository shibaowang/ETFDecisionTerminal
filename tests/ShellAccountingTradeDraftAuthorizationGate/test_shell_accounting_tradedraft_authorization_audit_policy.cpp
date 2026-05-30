#include "ShellAccountingTradeDraftAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs98Text(sourceRoot(argc, argv)) + docs99Text(sourceRoot(argc, argv));
    return containsAllTokens(text,
               {"must emit sanitized audit event",
                "audit failure policy",
                "audit is not trade confirmation",
                "audit is not broker order"})
        ? 0
        : 1;
}
