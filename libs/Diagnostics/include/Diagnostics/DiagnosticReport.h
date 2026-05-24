#pragma once

#include "Diagnostics/DiagnosticIssue.h"

#include <string>
#include <vector>

namespace etfdt::diagnostics {

struct DiagnosticServiceStatus final {
    std::string serviceName;
    bool enabled = false;
    bool supported = false;
    std::string executablePath;
    bool executableExists = false;
    std::string workingDirectory;
    bool workingDirectoryExists = false;
    std::string socketName;
    bool socketNamePresent = false;
    bool healthCheckSupported = false;
    bool canStart = false;
    std::vector<DiagnosticIssue> issues;
};

struct DiagnosticReport final {
    std::string configPath;
    std::string version;
    int totalServices = 0;
    int enabledServices = 0;
    int disabledServices = 0;
    int errorCount = 0;
    int warningCount = 0;
    std::vector<DiagnosticServiceStatus> services;
};

}  // namespace etfdt::diagnostics
