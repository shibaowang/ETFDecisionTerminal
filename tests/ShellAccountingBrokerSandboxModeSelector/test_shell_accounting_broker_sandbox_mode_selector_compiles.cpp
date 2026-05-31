#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"
#include "ShellAccountingBrokerSandboxModeSelector.h"

using namespace etfdt::dataservice;
using namespace etfdt::tests::shell_accounting_broker_sandbox_mode_selector;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return expect(shellAccountingBrokerOrderPortModeFromString("disabled") == ShellAccountingBrokerOrderPortMode::Disabled,
                  "selector must compile and parse disabled mode") &&
            expect(selectorSourceText(root).find("ShellAccountingBrokerOrderPortModeSelector") != std::string::npos,
                   "selector source must exist")
        ? 0
        : 1;
}
