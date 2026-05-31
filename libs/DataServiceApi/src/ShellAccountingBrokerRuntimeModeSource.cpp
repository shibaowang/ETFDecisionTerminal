#include "DataServiceApi/ShellAccountingBrokerRuntimeModeSource.h"

namespace etfdt::dataservice {

ShellAccountingBrokerOrderPortMode
DisabledShellAccountingBrokerRuntimeModeSource::brokerOrderPortMode() const noexcept
{
    return ShellAccountingBrokerOrderPortMode::Disabled;
}

ShellAccountingBrokerRuntimeModeSource& defaultShellAccountingBrokerRuntimeModeSource() noexcept
{
    static DisabledShellAccountingBrokerRuntimeModeSource source;
    return source;
}

}  // namespace etfdt::dataservice
