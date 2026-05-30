#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs107Text(root) + docs108Text(root);
    return containsAllTokens(docs, {
        "Broker order can only use an already confirmed TradeDraft / ledger fact",
        "confirmed TradeDraft id",
        "confirmed-ledger status",
        "no bypass of TASK-148 TradeDraft creation or TASK-150 confirmation",
    }) ? 0 : 1;
}
