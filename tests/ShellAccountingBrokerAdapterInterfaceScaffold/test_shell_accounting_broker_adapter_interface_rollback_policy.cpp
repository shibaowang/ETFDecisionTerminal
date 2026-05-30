#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAllTokens(docs114Text(sourceRoot(argc, argv)), {
        "Rollback Strategy",
        "disabled/null adapter",
        "does not affect TASK-154 dry-run",
        "does not affect TradeDraft create",
        "does not affect TradeDraft confirm",
        "A future real implementation must be a separate TASK",
    }) ? 0 : 1;
}
