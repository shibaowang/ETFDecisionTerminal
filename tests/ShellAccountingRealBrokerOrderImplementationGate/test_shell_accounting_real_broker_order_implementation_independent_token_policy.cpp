#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "independent authorization token",
        "TASK-154 dry-run authorization is not sufficient",
        "authorization missing fails",
        "disabled state fails",
    }) ? 0 : 1;
}
