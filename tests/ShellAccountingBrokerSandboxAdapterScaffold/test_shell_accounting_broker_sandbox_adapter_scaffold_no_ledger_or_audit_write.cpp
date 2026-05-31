#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto source = scaffoldSourceText(sourceRoot(argc, argv));
    return expect(!containsAnyToken(source, forbiddenWriteTableTokens()),
                  "sandbox scaffold must not write audit, ledger, execution group, or draft tables")
        ? 0
        : 1;
}
