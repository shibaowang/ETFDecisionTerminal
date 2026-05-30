#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAllTokens(docsText(sourceRoot(argc, argv)), {
        "Rollback / Kill Switch / Incident Containment Policy",
        "kill switch",
        "TASK-154 dry-run",
        "TradeDraft creation",
        "TradeDraft confirmation",
        "no blind retries",
        "not fabricate broker success",
    }) ? 0 : 1;
}
