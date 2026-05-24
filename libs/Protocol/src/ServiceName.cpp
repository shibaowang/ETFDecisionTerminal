#include "Protocol/ServiceName.h"

#include <array>
#include <string>

namespace etfdt::protocol {
namespace {

constexpr std::array<std::pair<ServiceName, std::string_view>, 6> kServiceNames = {
    {{ServiceName::ETFDecisionShell, "ETFDecisionShell"},
     {ServiceName::ETFWatchdog, "ETFWatchdog"},
     {ServiceName::ETFDataService, "ETFDataService"},
     {ServiceName::ETFMarketService, "ETFMarketService"},
     {ServiceName::ETFStrategyService, "ETFStrategyService"},
     {ServiceName::ETFAlertService, "ETFAlertService"}}};

}  // namespace

std::string_view toString(ServiceName value) noexcept
{
    for (const auto& entry : kServiceNames) {
        if (entry.first == value) {
            return entry.second;
        }
    }
    return {};
}

etfdt::core::Result<ServiceName> serviceNameFromString(std::string_view value)
{
    for (const auto& entry : kServiceNames) {
        if (entry.second == value) {
            return etfdt::core::Result<ServiceName>::success(entry.first);
        }
    }
    return etfdt::core::Result<ServiceName>::failure(
        "Unknown ServiceName: " + std::string(value));
}

template <>
etfdt::core::Result<ServiceName> fromString<ServiceName>(std::string_view value)
{
    return serviceNameFromString(value);
}

}  // namespace etfdt::protocol
