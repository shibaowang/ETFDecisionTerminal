#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs107Text(root) + docs108Text(root);
    return containsAllTokens(docs, {
        "broker disabled",
        "authorization missing",
        "confirmed trade missing",
        "duplicate order attempt",
        "stale draft / stale ledger",
        "broker unavailable",
        "broker rejected",
        "timeout",
        "rollback",
        "no silent success",
    }) ? 0 : 1;
}
