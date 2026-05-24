#pragma once

#include <string>
#include <vector>

namespace etfdt::watchdog {

struct ServiceProcessConfig final {
    std::string serviceName;
    std::string executablePath;
    std::vector<std::string> arguments;
    std::string socketName;
    std::string workingDirectory;
    int startupTimeoutMs = 5000;
    int shutdownTimeoutMs = 3000;
    int healthTimeoutMs = 2000;
    bool autoRestart = false;
    bool enabled = true;
};

}  // namespace etfdt::watchdog
