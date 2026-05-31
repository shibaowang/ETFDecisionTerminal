#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto actions = dataServiceActionsText(sourceRoot(argc, argv));
    return expect(actions.find("ShellAccountingBrokerOrderPortModeSelector") == std::string::npos,
                  "DataServiceActions must not include mode selector") &&
            expect(actions.find("shellAccountingBrokerOrderPortForMode") == std::string::npos,
                   "DataServiceActions runtime must not call selector") &&
            expect(actions.find("defaultShellAccountingBrokerOrderPort().submitOrderEnvelope") != std::string::npos,
                   "DataServiceActions must preserve default disabled provider call")
        ? 0
        : 1;
}
