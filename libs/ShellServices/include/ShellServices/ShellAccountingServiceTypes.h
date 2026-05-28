#pragma once

#include "ShellServices/ShellAccountingIssue.h"

#include <string>
#include <vector>

namespace etfdt::shell_services {

struct ShellAccountingServiceRequest final {
    std::string actionName;
    std::string accountId;
    std::string portfolioId;
    std::string sourceFromTime;
    std::string sourceToTime;
    bool includeIssues = true;
    bool includeDebugMetadata = false;
    std::string calculationVersion;
    std::vector<std::string> requestedOutputs;
    int timeoutMs = 0;
};

struct ShellAccountingServiceResult final {
    std::string actionName;
    bool protocolSuccess = true;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string payloadStatus;
    std::string dataQualityStatus = "UNAVAILABLE";
    std::vector<ShellAccountingIssue> issues;
    std::vector<ShellAccountingIssue> warnings;
    std::vector<ShellAccountingIssue> errors;
    std::string rawPayload;
    bool timeout = false;
    bool transportError = false;
    bool protocolError = false;
    bool domainError = false;
    bool generatedTradeDraft = false;
    bool generatedTradeSuggestion = false;
    bool strategyExecuted = false;
    bool brokerOrderSubmitted = false;
};

[[nodiscard]] bool isShellAccountingServiceResultReadOnly(
    const ShellAccountingServiceResult& result) noexcept;

}  // namespace etfdt::shell_services
