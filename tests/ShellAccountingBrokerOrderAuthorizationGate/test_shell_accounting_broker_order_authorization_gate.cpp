#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = docs107Text(root);
    const auto plan = docs108Text(root);
    return containsAllTokens(gate + plan, {
        "TASK-119",
        "TASK-152",
        "Future broker order implementation must be a separate",
        "Broker order is not strategy execution",
        "Broker order is not automatic trading",
        "Broker order must not accept raw QML payload",
    }) ? 0 : 1;
}
