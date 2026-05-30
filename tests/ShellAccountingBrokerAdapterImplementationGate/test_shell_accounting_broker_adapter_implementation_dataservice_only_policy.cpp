#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "DataService-Only Boundary Policy",
        "DataService remains the only allowed broker order entry point",
        "QML",
        "Presenter",
        "ShellServices",
        "must not directly",
    }) ? 0 : 1;
}
