#pragma once

#include "Watchdog/ServiceManifest.h"
#include "Watchdog/ServiceManifestLoader.h"
#include "Watchdog/ServiceManifestStatus.h"
#include "Watchdog/ServiceProcessConfig.h"
#include "Watchdog/ServiceProcessManager.h"
#include "Watchdog/ServiceProcessStatus.h"

namespace etfdt::watchdog {

constexpr const char* kWatchdogModuleName = "ETFWatchdogLib";

}  // namespace etfdt::watchdog
