#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "disabled",
        "credentials missing",
        "mode mismatch",
        "broker unavailable",
        "broker rejected",
        "timeout",
        "duplicate order",
        "unknown broker state",
        "audit failure",
        "reconciliation required",
        "no silent success",
    }) ? 0 : 1;
}
