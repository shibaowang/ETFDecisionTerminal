#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "idempotency key",
        "duplicate order attempt",
        "must not place a second order",
        "duplicate must be audited",
    }) ? 0 : 1;
}
