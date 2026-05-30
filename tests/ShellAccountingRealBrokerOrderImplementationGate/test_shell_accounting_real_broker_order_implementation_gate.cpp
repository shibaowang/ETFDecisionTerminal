#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs112Text(root) + docs113Text(root);
    return containsAllTokens(docs, {
        "TASK-119",
        "TASK-155",
        "Future real broker order implementation must be a separate TASK",
        "DataService-only",
        "TASK-154 dry-run",
        "final user confirmation",
        "independent authorization token",
        "kill switch",
    }) ? 0 : 1;
}
