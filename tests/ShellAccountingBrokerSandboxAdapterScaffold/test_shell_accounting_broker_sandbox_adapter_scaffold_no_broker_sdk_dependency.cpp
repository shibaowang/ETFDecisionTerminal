#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto source = scaffoldSourceText(sourceRoot(argc, argv));
    return expect(!containsAnyToken(source, brokerSdkTokens()),
                  "sandbox scaffold must not import or reference broker SDK dependencies")
        ? 0
        : 1;
}
