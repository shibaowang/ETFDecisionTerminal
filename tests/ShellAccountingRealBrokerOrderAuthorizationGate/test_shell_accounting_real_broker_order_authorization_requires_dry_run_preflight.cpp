#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "TASK-154 dry-run boundary remains present",
        "successful dry-run result required",
        "dry-run reference",
        "missing dry-run must fail",
    }) ? 0 : 1;
}
