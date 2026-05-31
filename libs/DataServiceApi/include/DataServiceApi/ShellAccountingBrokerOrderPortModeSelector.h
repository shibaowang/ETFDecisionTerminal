#pragma once

#include "DataServiceApi/ShellAccountingBrokerOrderPort.h"

#include <string_view>

namespace etfdt::dataservice {

enum class ShellAccountingBrokerOrderPortMode {
    Disabled,
    Sandbox,
    Unsupported,
};

[[nodiscard]] ShellAccountingBrokerOrderPortMode defaultShellAccountingBrokerOrderPortMode() noexcept;

[[nodiscard]] ShellAccountingBrokerOrderPortMode shellAccountingBrokerOrderPortModeFromString(
    std::string_view mode) noexcept;

[[nodiscard]] ShellAccountingBrokerOrderPort& shellAccountingBrokerOrderPortForMode(
    ShellAccountingBrokerOrderPortMode mode) noexcept;

[[nodiscard]] ShellAccountingBrokerOrderPort& shellAccountingBrokerOrderPortForMode(
    std::string_view mode) noexcept;

}  // namespace etfdt::dataservice
