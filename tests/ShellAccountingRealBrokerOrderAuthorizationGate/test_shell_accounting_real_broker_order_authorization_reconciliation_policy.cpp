#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "order accepted",
        "order rejected",
        "order pending",
        "order unknown",
        "broker order id storage separately authorized",
        "fill reconciliation separately authorized",
        "correction flow separately authorized",
    }) ? 0 : 1;
}
