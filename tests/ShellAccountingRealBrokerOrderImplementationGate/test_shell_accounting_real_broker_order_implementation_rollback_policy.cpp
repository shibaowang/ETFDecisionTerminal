#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "disabling real placement",
        "TradeDraft create",
        "TradeDraft confirm",
        "production trading UI",
        "TASK-154 dry-run",
        "avoid blind retries",
        "strategy fallback",
        "automatic trading fallback",
        "fake broker success",
    }) ? 0 : 1;
}
