#pragma once

#include "ShellServices/ShellAccountingIssue.h"

#include <cstdint>
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
    std::string strategyId;
    std::string strategyCode;
    std::string instrumentId;
    std::string instrumentCode;
    std::string side;
    std::int64_t quantity1e6 = 0;
    std::string reason;
    std::string source;
    std::string sourceSnapshotId;
    std::string sourceReplayId;
    std::int64_t draftId = 0;
    std::string authorization;
    bool riskChecked = true;
    bool riskBlocked = false;
    bool tradeDraftDisabled = false;
    bool confirmationDisabled = false;
    std::string securityCode;
    std::string tradeSide;
    std::string quantityUnits;
    std::string priceAmountMinor;
    std::string grossAmountMinor;
    std::string feeAmountMinor;
    std::string taxAmountMinor;
    std::string occurredAt;
    std::string sourceMemo;
    std::string requestId;
    std::string idempotencyKey;
    std::string movementType;
    std::string amountMinor;
    std::string currency;
    std::string sourceReference;
};

struct ShellAccountingServiceResult final {
    std::string actionName;
    bool protocolSuccess = true;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string payloadStatus;
    std::string dataQualityStatus = "UNAVAILABLE";
    bool hasRows = false;
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
