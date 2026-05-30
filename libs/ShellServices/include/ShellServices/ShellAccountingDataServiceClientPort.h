#pragma once

#include "ShellServices/ShellAccountingIssue.h"

#include <string>
#include <vector>

namespace etfdt::shell_services {

struct ShellAccountingDataServiceClientRequest final {
    std::string actionName;
    std::string payloadJson;
    int timeoutMs = 0;
};

struct ShellAccountingDataServiceClientResponse final {
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
    std::string errorMessage;
};

class ShellAccountingDataServiceClientPort {
public:
    virtual ~ShellAccountingDataServiceClientPort() = default;

    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callPositionList(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callCashSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftCreate(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftConfirm(
        const ShellAccountingDataServiceClientRequest& request);
};

}  // namespace etfdt::shell_services
