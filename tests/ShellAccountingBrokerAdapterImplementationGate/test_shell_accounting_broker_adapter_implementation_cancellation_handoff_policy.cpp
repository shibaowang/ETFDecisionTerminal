#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "Cancellation / Correction Handoff Policy",
        "Cancel",
        "replace",
        "correct",
        "reversal",
        "separate tasks",
    }) ? 0 : 1;
}
