#include "DataServiceApi/ShellAccountingBrokerOrderPortModeSelector.h"

#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"
#include "DataServiceApi/ShellAccountingBrokerSandboxOrderPortScaffold.h"

namespace etfdt::dataservice {

ShellAccountingBrokerOrderPortMode defaultShellAccountingBrokerOrderPortMode() noexcept
{
    return ShellAccountingBrokerOrderPortMode::Disabled;
}

ShellAccountingBrokerOrderPortMode shellAccountingBrokerOrderPortModeFromString(std::string_view mode) noexcept
{
    if (mode == "disabled") {
        return ShellAccountingBrokerOrderPortMode::Disabled;
    }
    if (mode == "sandbox") {
        return ShellAccountingBrokerOrderPortMode::Sandbox;
    }
    return ShellAccountingBrokerOrderPortMode::Unsupported;
}

ShellAccountingBrokerOrderPort& shellAccountingBrokerOrderPortForMode(
    ShellAccountingBrokerOrderPortMode mode) noexcept
{
    if (mode == ShellAccountingBrokerOrderPortMode::Sandbox) {
        static ShellAccountingBrokerSandboxOrderPortScaffold sandboxScaffold;
        return sandboxScaffold;
    }
    return defaultShellAccountingBrokerOrderPort();
}

ShellAccountingBrokerOrderPort& shellAccountingBrokerOrderPortForMode(std::string_view mode) noexcept
{
    return shellAccountingBrokerOrderPortForMode(shellAccountingBrokerOrderPortModeFromString(mode));
}

}  // namespace etfdt::dataservice
