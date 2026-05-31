#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto source = scaffoldSourceText(sourceRoot(argc, argv));
    return expect(source.find("Fake") == std::string::npos, "sandbox scaffold must not reuse fake adapter naming") &&
            expect(source.find("test-only") == std::string::npos,
                   "sandbox scaffold production source must not embed test-only fake behavior")
        ? 0
        : 1;
}
