#include "ShellAccountingBrokerSandboxAdapterAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_authorization_gate;

int main(int argc, char** argv)
{
    const auto text = readTextFile(sourceRoot(argc, argv) / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");
    return containsAllTokens(text, {
        "accounting.broker_order.dry_run",
        "\\\"dryRun\\\":true",
        "\\\"brokerDisabled\\\":true",
        "\\\"dryRunOnly\\\":true",
        "\\\"brokerOrderId\\\":null",
    }) ? 0 : 1;
}
