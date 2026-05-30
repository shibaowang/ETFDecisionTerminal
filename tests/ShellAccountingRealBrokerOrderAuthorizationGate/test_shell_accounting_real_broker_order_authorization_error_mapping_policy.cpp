#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "broker disabled",
        "authorization missing",
        "dry-run missing",
        "confirmed trade missing",
        "final confirmation missing",
        "stale quote",
        "market closed",
        "risk guard failed",
        "duplicate order",
        "broker unavailable",
        "broker rejected",
        "timeout",
        "unknown broker state",
        "audit failure",
        "reconciliation required",
        "no silent success",
    }) ? 0 : 1;
}
