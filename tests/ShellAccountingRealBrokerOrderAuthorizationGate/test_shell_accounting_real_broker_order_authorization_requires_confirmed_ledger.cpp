#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "confirmed TradeDraft / ledger fact",
        "confirmed ledger input",
        "no raw UI payload order",
        "no unconfirmed Draft order",
        "no stale ledger order",
    }) ? 0 : 1;
}
