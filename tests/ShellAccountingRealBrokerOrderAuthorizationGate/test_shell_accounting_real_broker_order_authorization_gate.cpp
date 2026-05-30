#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs110Text(root) + docs111Text(root);
    return containsAllTokens(docs, {
        "TASK-119",
        "TASK-154",
        "real broker order implementation must be a separate TASK",
        "DataService-only",
        "dry-run result required",
        "final user confirmation required",
        "independent authorization token",
    }) ? 0 : 1;
}
