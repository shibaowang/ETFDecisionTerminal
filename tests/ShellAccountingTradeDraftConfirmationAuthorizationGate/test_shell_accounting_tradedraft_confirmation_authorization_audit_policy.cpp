#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = docs101Text(sourceRoot(argc, argv)) + "\n" + docs102Text(sourceRoot(argc, argv));
    return containsAllTokens(text, {
        "sanitized audit event",
        "audit failure rollback",
        "audit is not trade execution replacement",
    }) ? 0 : 1;
}
