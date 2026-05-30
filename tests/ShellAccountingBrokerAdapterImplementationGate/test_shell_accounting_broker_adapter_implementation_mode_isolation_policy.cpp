#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "Adapter Mode Policy",
        "disabled",
        "fake test-only",
        "sandbox",
        "paper",
        "real",
        "Mode mismatch must fail closed",
    }) ? 0 : 1;
}
