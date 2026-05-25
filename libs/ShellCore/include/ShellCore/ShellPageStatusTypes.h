#pragma once

#include <string>

namespace etfdt::shell {

struct ShellPageStatus final {
    std::string pageKey;
    std::string title;
    std::string moduleStatus;
    std::string dataMode;
    std::string connectionStatus;
    bool placeholder = true;
    int warningCount = 0;
    int errorCount = 0;
    std::string statusText;
    std::string detailText;
};

struct ShellMockLogEntry final {
    std::string timeText;
    std::string level;
    std::string source;
    std::string message;
};

}  // namespace etfdt::shell
