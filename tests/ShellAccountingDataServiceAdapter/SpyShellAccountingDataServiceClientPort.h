#pragma once

#include "ShellServices/ShellAccountingDataServiceClientPort.h"
#include "ShellServices/ShellAccountingServiceTypes.h"

#include <map>
#include <string>
#include <vector>

namespace etfdt::shell_services::tests {

class SpyShellAccountingDataServiceClientPort final
    : public ShellAccountingDataServiceClientPort {
public:
    void setNextResponse(ShellAccountingDataServiceClientResponse response);

    [[nodiscard]] ShellAccountingDataServiceClientResponse callPositionList(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callCashSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const ShellAccountingDataServiceClientRequest& request) override;

    [[nodiscard]] int callCount(const std::string& methodName) const;
    [[nodiscard]] int totalCallCount() const;
    [[nodiscard]] const ShellAccountingDataServiceClientRequest& lastRequest() const;
    [[nodiscard]] const std::vector<std::string>& calledMethods() const noexcept;

    void dataAuditAppend();
    void tradeWriteAction();
    void draftAction();
    void snapshotWriteAction();
    void strategyAction();
    void brokerAction();

    [[nodiscard]] bool forbiddenActionCalled() const noexcept;
    [[nodiscard]] bool generatedTradeDraft() const noexcept;
    [[nodiscard]] bool generatedTradeSuggestion() const noexcept;
    [[nodiscard]] bool strategyExecuted() const noexcept;
    [[nodiscard]] bool brokerOrderSubmitted() const noexcept;

private:
    ShellAccountingDataServiceClientResponse record(
        std::string methodName,
        const ShellAccountingDataServiceClientRequest& request);

    ShellAccountingDataServiceClientResponse nextResponse_;
    ShellAccountingDataServiceClientRequest lastRequest_;
    std::vector<std::string> calledMethods_;
    std::map<std::string, int> callCounts_;
    bool forbiddenActionCalled_ = false;
    bool generatedTradeDraft_ = false;
    bool generatedTradeSuggestion_ = false;
    bool strategyExecuted_ = false;
    bool brokerOrderSubmitted_ = false;
};

[[nodiscard]] ShellAccountingServiceRequest makeLiveCallSkeletonRequest(std::string actionName);
[[nodiscard]] ShellAccountingDataServiceClientResponse makeOkClientResponse(std::string actionName);
[[nodiscard]] ShellAccountingIssue makePortIssue(
    std::string code,
    std::string level,
    bool blocking);

}  // namespace etfdt::shell_services::tests
