#pragma once

#include "DataServiceApi/ShellAccountingBrokerOrderPort.h"

namespace etfdt::dataservice {

[[nodiscard]] ShellAccountingBrokerOrderPort& defaultShellAccountingBrokerOrderPort() noexcept;

}  // namespace etfdt::dataservice
