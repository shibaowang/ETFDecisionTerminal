#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "Reconciliation Handoff Policy",
        "Broker adapter implementation does not automatically authorize reconciliation",
        "Broker order id storage",
        "fill reconciliation",
        "partial fill handling",
    }) ? 0 : 1;
}
