#include "DataServiceApi/ShellAccountingBrokerRuntimeModeSource.h"

namespace etfdt::dataservice {

ShellAccountingBrokerOrderPortMode
DisabledShellAccountingBrokerRuntimeModeSource::brokerOrderPortMode() const noexcept
{
    return ShellAccountingBrokerOrderPortMode::Disabled;
}

ShellAccountingBrokerOrderPortMode
SandboxShellAccountingBrokerRuntimeModeSourceScaffold::brokerOrderPortMode() const noexcept
{
    return ShellAccountingBrokerOrderPortMode::Sandbox;
}

ShellAccountingBrokerRuntimeModeSource& defaultShellAccountingBrokerRuntimeModeSource() noexcept
{
    static DisabledShellAccountingBrokerRuntimeModeSource source;
    return source;
}

ShellAccountingBrokerRuntimeModeSource& shellAccountingBrokerRuntimeModeSourceForMode(
    ShellAccountingBrokerOrderPortMode mode) noexcept
{
    if (mode == ShellAccountingBrokerOrderPortMode::Sandbox) {
        static SandboxShellAccountingBrokerRuntimeModeSourceScaffold source;
        return source;
    }
    return defaultShellAccountingBrokerRuntimeModeSource();
}

}  // namespace etfdt::dataservice
