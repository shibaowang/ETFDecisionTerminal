#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "market / account / instrument",
        "side / quantity / price",
        "max quantity / max notional guard",
        "stale quote guard",
        "market hours guard",
        "kill switch",
    }) ? 0 : 1;
}
