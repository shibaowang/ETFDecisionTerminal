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

}  // namespace etfdt::dataservice
