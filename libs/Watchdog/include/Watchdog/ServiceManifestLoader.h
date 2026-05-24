#pragma once

#include "Watchdog/ServiceManifest.h"
#include "Watchdog/ServiceProcessManager.h"

#include <filesystem>
#include <string>

namespace etfdt::watchdog {

class ServiceManifestLoader final {
public:
    [[nodiscard]] static WatchdogResult<ServiceManifest> loadFromFile(
        const std::filesystem::path& path);
    [[nodiscard]] static WatchdogResult<ServiceManifest> loadFromJsonString(
        const std::string& json);
    [[nodiscard]] static ServiceManifestValidationResult validate(
        const ServiceManifest& manifest);
};

}  // namespace etfdt::watchdog
