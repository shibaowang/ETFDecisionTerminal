#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    const auto docs = docsText(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "TASK-119",
        "TASK-157",
        "Future broker adapter implementation must be a separate TASK",
        "default disabled",
        "DataService-only",
        "no broker network call",
        "credentials",
        "fake test-only",
        "no silent success",
    }) ? 0 : 1;
}
