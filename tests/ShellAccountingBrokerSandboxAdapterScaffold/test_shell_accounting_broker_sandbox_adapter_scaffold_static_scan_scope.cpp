#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = task161DocsText(root);
    const auto source = scaffoldSourceText(root);
    return expect(docs.find("Documentation and tests may mention policy words") != std::string::npos,
                  "TASK-161 docs must distinguish policy words from implementation tokens") &&
            expect(!containsAnyToken(source, brokerNetworkTokens()),
                   "production scaffold source must remain separately scannable for network tokens") &&
            expect(!containsAnyToken(source, credentialValueTokens()),
                   "production scaffold source must remain separately scannable for credential values")
        ? 0
        : 1;
}
