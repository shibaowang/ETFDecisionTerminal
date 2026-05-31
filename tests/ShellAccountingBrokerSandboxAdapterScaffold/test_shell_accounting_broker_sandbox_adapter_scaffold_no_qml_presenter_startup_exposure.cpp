#include "ShellAccountingBrokerSandboxAdapterScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_adapter_scaffold;

int main(int argc, char** argv)
{
    const auto source = productionUiPathText(sourceRoot(argc, argv));
    return expect(!containsAnyToken(source, qmlPresenterExposureTokens()),
                  "QML, Presenter, ShellServices, ShellCore, and startup must not expose sandbox adapter objects")
        ? 0
        : 1;
}
