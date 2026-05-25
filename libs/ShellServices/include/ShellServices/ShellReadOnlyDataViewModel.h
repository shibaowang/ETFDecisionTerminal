#pragma once

#include <string>

namespace etfdt::shell_services {

struct ShellReadOnlyDataViewModel final {
    bool healthy = false;
    std::string summaryText;
    int accountCount = 0;
    int portfolioCount = 0;
    int instrumentCount = 0;
    int strategyCount = 0;
    std::string lastError;
};

}  // namespace etfdt::shell_services
