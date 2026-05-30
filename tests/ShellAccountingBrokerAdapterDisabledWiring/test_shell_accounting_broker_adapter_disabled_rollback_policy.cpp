#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    return containsAllTokens(docs117Text(sourceRoot(argc, argv)), {
        "Rollback Strategy",
        "disable broker adapter wiring",
        "TASK-154 dry-run",
        "TradeDraft create",
        "TradeDraft confirm",
        "must not create fake broker success",
    }) ? 0 : 1;
}
