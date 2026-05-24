#pragma once

#include "Watchdog/ServiceProcessConfig.h"

#include <string>
#include <vector>

namespace etfdt::watchdog {

struct ServiceManifest final {
    std::string version;
    std::vector<ServiceProcessConfig> services;
};

struct ServiceManifestError final {
    std::string message;
};

struct ServiceManifestValidationResult final {
    bool valid = true;
    std::vector<ServiceManifestError> errors;
    std::vector<std::string> warnings;
};

}  // namespace etfdt::watchdog
