#pragma once

#include "CoreDomain/Result.h"

#include <string_view>

namespace etfdt::protocol {

enum class ServiceName {
    ETFDecisionShell,
    ETFWatchdog,
    ETFDataService,
    ETFMarketService,
    ETFStrategyService,
    ETFAlertService,
};

[[nodiscard]] std::string_view toString(ServiceName value) noexcept;
[[nodiscard]] etfdt::core::Result<ServiceName> serviceNameFromString(std::string_view value);

template <typename T>
[[nodiscard]] etfdt::core::Result<T> fromString(std::string_view value);

template <>
[[nodiscard]] etfdt::core::Result<ServiceName> fromString<ServiceName>(std::string_view value);

}  // namespace etfdt::protocol
