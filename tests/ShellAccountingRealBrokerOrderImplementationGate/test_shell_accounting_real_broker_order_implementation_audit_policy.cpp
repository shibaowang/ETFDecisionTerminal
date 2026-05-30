#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "sanitized order intent",
        "dry-run reference",
        "confirmed ledger reference",
        "user confirmation reference",
        "no raw broker payload",
        "no credentials",
        "no raw SQL",
    }) ? 0 : 1;
}
