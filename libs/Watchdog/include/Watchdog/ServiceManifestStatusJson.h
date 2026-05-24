#pragma once

#include "Watchdog/ServiceManifestStatus.h"
#include "Watchdog/ServiceProcessManager.h"

#include <QJsonObject>

#include <filesystem>
#include <string>

namespace etfdt::watchdog {

[[nodiscard]] QJsonObject toJsonObject(const ManifestStatusReport& report);
[[nodiscard]] std::string toJsonString(const ManifestStatusReport& report, bool pretty);
[[nodiscard]] WatchdogResult<bool> writeManifestStatusReportJson(
    const ManifestStatusReport& report,
    const std::filesystem::path& outputPath,
    bool pretty = true);

}  // namespace etfdt::watchdog
