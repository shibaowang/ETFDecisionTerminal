#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "accepted / rejected / pending / unknown broker",
        "Broker order id storage requires explicit policy",
        "Fill reconciliation",
        "Correction / cancel flow",
        "reconciliation required",
    }) ? 0 : 1;
}
