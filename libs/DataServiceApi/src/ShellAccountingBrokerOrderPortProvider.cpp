#include "DataServiceApi/ShellAccountingBrokerOrderPortProvider.h"

namespace etfdt::dataservice {

ShellAccountingBrokerOrderPort& defaultShellAccountingBrokerOrderPort() noexcept
{
    static DisabledShellAccountingBrokerOrderPort disabledPort;
    return disabledPort;
}

}  // namespace etfdt::dataservice
