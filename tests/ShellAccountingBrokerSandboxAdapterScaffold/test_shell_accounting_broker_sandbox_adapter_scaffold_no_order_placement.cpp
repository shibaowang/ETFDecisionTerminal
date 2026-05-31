#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto source = scaffoldSourceText(sourceRoot(argc, argv));
    return expect(!containsAnyToken(source, realOrderPlacementTokens()),
                  "sandbox scaffold must not contain real order placement tokens")
        ? 0
        : 1;
}
