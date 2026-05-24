#pragma once

#include "Watchdog/ServiceManifest.h"

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace etfdt::watchdog {

enum class ServiceConfigIssueLevel {
    Info,
    Warning,
    Error,
};

struct ServiceConfigIssue final {
    ServiceConfigIssueLevel level = ServiceConfigIssueLevel::Info;
    std::string code;
    std::string message;
};

struct ServiceConfigStatus final {
    std::string serviceName;
    bool enabled = false;
    bool supported = false;
    std::string executablePath;
    bool executableExists = false;
    std::string workingDirectory;
    bool workingDirectoryExists = false;
    std::string socketName;
    bool socketNamePresent = false;
    bool autoRestart = false;
    bool autoRestartEnabledButIgnored = false;
    bool healthCheckSupported = false;
    bool canStart = false;
    std::vector<ServiceConfigIssue> issues;
};

struct ManifestStatusReport final {
    std::string configPath;
    std::string version;
    std::size_t totalServices = 0;
    std::size_t enabledServices = 0;
    std::size_t disabledServices = 0;
    std::size_t errorCount = 0;
    std::size_t warningCount = 0;
    std::vector<ServiceConfigStatus> serviceStatuses;
};

struct ManifestStatusOptions final {
    std::string configPath;
    std::filesystem::path baseDirectory = std::filesystem::current_path();
};

[[nodiscard]] std::string_view toString(ServiceConfigIssueLevel level) noexcept;
[[nodiscard]] ManifestStatusReport buildManifestStatusReport(
    const ServiceManifest& manifest,
    const ManifestStatusOptions& options = {});

}  // namespace etfdt::watchdog
