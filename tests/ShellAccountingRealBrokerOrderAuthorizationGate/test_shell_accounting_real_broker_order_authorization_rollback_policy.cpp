#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "Rollback / Incident Containment Policy",
        "disable real broker order",
        "keep TradeDraft create / confirm unaffected",
        "keep TASK-154 dry-run unaffected",
        "no fake broker success",
        "no strategy fallback",
        "no automatic trading fallback",
    }) ? 0 : 1;
}
