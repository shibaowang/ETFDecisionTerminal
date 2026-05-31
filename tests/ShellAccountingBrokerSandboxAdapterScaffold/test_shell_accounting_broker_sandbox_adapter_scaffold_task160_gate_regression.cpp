#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto docs = task160DocsText(sourceRoot(argc, argv));
    return expect(containsAllTokens(docs,
                      {"TASK-160", "Disabled Default Policy", "Credentials Isolation Policy",
                          "CI No-Network Policy", "DataService-Only Broker Adapter Boundary",
                          "No Real Order Placement Policy", "No Strategy Execution / Automatic Trading Policy"}),
                  "TASK-160 authorization gate docs must remain intact")
        ? 0
        : 1;
}
