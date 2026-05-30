#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "sanitized order intent",
        "dry-run reference",
        "confirmed trade reference",
        "user confirmation reference",
        "no raw broker payload",
        "no credentials",
        "no raw SQL",
        "no full `trade_log` payload",
    }) ? 0 : 1;
}
