#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(docsText(root), {
        "Order ID Policy",
        "must not produce, persist, or expose a real broker order id",
        "Future real order id storage requires a separate authorization task",
    }) && !containsAnyToken(productionText(root), realOrderIdTokens()) ? 0 : 1;
}
