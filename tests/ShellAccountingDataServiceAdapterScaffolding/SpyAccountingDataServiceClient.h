#pragma once

#include "ShellServices/ShellAccountingIssue.h"
#include "ShellServices/ShellAccountingServiceTypes.h"

#include <map>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding {

struct SpyProtocolIssue final {
    std::string code;
    std::string level;
    std::string message;
    bool blocking = false;
};

struct SpyProtocolResponse final {
    bool success = true;
    std::string action;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string status;
    std::string dataQualityStatus = "UNAVAILABLE";
    std::vector<SpyProtocolIssue> issues;
    std::vector<SpyProtocolIssue> warnings;
    std::vector<SpyProtocolIssue> errors;
    std::string rawPayload = "{}";
    bool malformed = false;
    bool timeout = false;
    bool transportError = false;
    bool serverUnavailable = false;
};

class SpyAccountingDataServiceClient final {
public:
    void setNextResponse(SpyProtocolResponse response);

    [[nodiscard]] SpyProtocolResponse positionList(std::string payloadJson, int timeoutMs);
    [[nodiscard]] SpyProtocolResponse cashSummary(std::string payloadJson, int timeoutMs);
    [[nodiscard]] SpyProtocolResponse portfolioPnlSummary(std::string payloadJson, int timeoutMs);
    [[nodiscard]] SpyProtocolResponse basePositionSummary(std::string payloadJson, int timeoutMs);
    [[nodiscard]] SpyProtocolResponse sniperPoolSummary(std::string payloadJson, int timeoutMs);

    void dataAuditAppend();
    void tradeDraftCreate();
    void snapshotWrite();
    void strategyExecute();
    void brokerOrderSubmit();

    [[nodiscard]] const std::vector<std::string>& calledActions() const noexcept;
    [[nodiscard]] const std::vector<std::string>& calledWrappers() const noexcept;
    [[nodiscard]] const std::map<std::string, std::string>& lastPayloadByAction() const noexcept;
    [[nodiscard]] const std::map<std::string, int>& lastTimeoutByAction() const noexcept;
    [[nodiscard]] bool forbiddenActionCalled() const noexcept;
    [[nodiscard]] bool writeActionCalled() const noexcept;
    [[nodiscard]] bool generatedTradeDraft() const noexcept;
    [[nodiscard]] bool generatedTradeSuggestion() const noexcept;
    [[nodiscard]] bool strategyExecuted() const noexcept;
    [[nodiscard]] bool brokerOrderSubmitted() const noexcept;

private:
    [[nodiscard]] SpyProtocolResponse recordReadOnlyCall(
        std::string actionName,
        std::string wrapperName,
        std::string payloadJson,
        int timeoutMs);

    SpyProtocolResponse nextResponse_;
    std::vector<std::string> calledActions_;
    std::vector<std::string> calledWrappers_;
    std::map<std::string, std::string> lastPayloadByAction_;
    std::map<std::string, int> lastTimeoutByAction_;
    bool forbiddenActionCalled_ = false;
    bool writeActionCalled_ = false;
    bool generatedTradeDraft_ = false;
    bool generatedTradeSuggestion_ = false;
    bool strategyExecuted_ = false;
    bool brokerOrderSubmitted_ = false;
};

[[nodiscard]] std::string buildFuturePayload(
    const etfdt::shell_services::ShellAccountingServiceRequest& request);

[[nodiscard]] etfdt::shell_services::ShellAccountingServiceResult mapSpyProtocolResponse(
    const SpyProtocolResponse& response);

}  // namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding
