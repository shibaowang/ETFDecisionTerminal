#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "DataService-only",
        "adapter must sit behind DataService",
        "QML must not directly call broker code",
        "Presenter must not directly call broker code",
        "DataAccess must not directly call",
    }) ? 0 : 1;
}
