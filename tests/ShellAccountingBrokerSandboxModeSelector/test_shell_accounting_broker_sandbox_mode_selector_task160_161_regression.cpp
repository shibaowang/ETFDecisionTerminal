#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto docs = task160And161DocsText(sourceRoot(argc, argv));
    return expect(containsAllTokens(docs,
                      {"TASK-160", "TASK-161", "Disabled Default Policy", "Credentials Isolation Policy",
                          "CI no-network", "not a usable broker adapter", "No Broker SDK / No Real Order Placement"}),
                  "TASK-160 and TASK-161 docs must remain valid")
        ? 0
        : 1;
}
