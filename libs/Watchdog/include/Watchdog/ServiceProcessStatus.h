#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace etfdt::watchdog {

enum class HealthState {
    Unknown,
    Starting,
    Healthy,
    Unhealthy,
    Stopped,
    Failed,
};

struct ServiceProcessStatus final {
    std::string serviceName;
    bool running = false;
    std::int64_t pid = 0;
    std::string lastStartTimeUtc;
    std::string lastStopTimeUtc;
    std::optional<int> lastExitCode;
    std::string lastErrorMessage;
    HealthState healthState = HealthState::Unknown;
    std::string message;
};

[[nodiscard]] std::string_view toString(HealthState state) noexcept;

}  // namespace etfdt::watchdog
