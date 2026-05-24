#pragma once

#include <string>
#include <vector>

namespace etfdt::shell {

struct ShellDiagnosticIssueRow final {
    std::string level;
    std::string code;
    std::string message;
    std::string displayText;
    int severityRank = 0;
};

struct ShellDiagnosticServiceRow final {
    std::string serviceName;
    bool enabled = false;
    bool supported = false;
    bool canStart = false;
    bool healthCheckSupported = false;
    bool executableExists = false;
    bool workingDirectoryExists = false;
    bool socketNamePresent = false;
    std::string statusText;
    std::string statusSeverity;
    int issueCount = 0;
    std::vector<ShellDiagnosticIssueRow> issues;
};

}  // namespace etfdt::shell
