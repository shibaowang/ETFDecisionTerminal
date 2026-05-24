#include "Watchdog/ServiceProcessStatus.h"

namespace etfdt::watchdog {

std::string_view toString(HealthState state) noexcept
{
    switch (state) {
    case HealthState::Unknown:
        return "UNKNOWN";
    case HealthState::Starting:
        return "STARTING";
    case HealthState::Healthy:
        return "HEALTHY";
    case HealthState::Unhealthy:
        return "UNHEALTHY";
    case HealthState::Stopped:
        return "STOPPED";
    case HealthState::Failed:
        return "FAILED";
    }
    return "UNKNOWN";
}

}  // namespace etfdt::watchdog
