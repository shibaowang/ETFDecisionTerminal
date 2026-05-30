#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docs112Text(sourceRoot(argc, argv)) + docs113Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "disabled",
        "authorization missing",
        "dry-run missing",
        "confirmed ledger missing",
        "final confirmation missing",
        "risk guard failed",
        "duplicate order",
        "credentials unavailable",
        "broker unavailable",
        "broker rejected",
        "timeout",
        "unknown broker state",
        "audit failure",
        "reconciliation required",
        "rollback",
        "no silent success",
    }) ? 0 : 1;
}
