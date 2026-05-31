#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    return expect(!containsAnyToken(productionUiPathText(sourceRoot(argc, argv)), qmlPresenterExposureTokens()),
                  "QML, startup, ShellServices, and ShellCore must not expose selector or sandbox adapter")
        ? 0
        : 1;
}
