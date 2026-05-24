#pragma once

#include "ShellCore/ShellDiagnosticRows.h"

#include <string>
#include <vector>

namespace etfdt::shell {

struct ShellDiagnosticViewModel final {
    bool ok = false;
    std::string title;
    std::string summaryText;
    int totalServices = 0;
    int enabledServices = 0;
    int disabledServices = 0;
    int errorCount = 0;
    int warningCount = 0;
    int blockedServiceCount = 0;
    std::vector<ShellDiagnosticServiceRow> serviceRows;
};

struct ShellDiagnosticLoadResult final {
    bool ok = false;
    ShellDiagnosticViewModel viewModel;
    std::string errorMessage;
};

}  // namespace etfdt::shell
