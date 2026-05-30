#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "distinct mode",
        "distinct credentials",
        "distinct audit marker",
        "no accidental real order in tests",
        "CI network calls disabled",
    }) ? 0 : 1;
}
