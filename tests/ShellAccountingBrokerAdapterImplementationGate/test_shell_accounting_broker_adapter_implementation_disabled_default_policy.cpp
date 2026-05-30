#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "Default-Disabled Policy",
        "default to disabled",
        "fail closed",
        "Missing configuration",
        "rollback mode",
    }) ? 0 : 1;
}
