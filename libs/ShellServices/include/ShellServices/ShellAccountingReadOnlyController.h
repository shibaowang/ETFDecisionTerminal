#pragma once

#include "ShellServices/ShellAccountingDisplayText.h"
#include "ShellServices/ShellAccountingIssue.h"
#include "ShellServices/ShellAccountingState.h"
#include "ShellServices/ShellAccountingServiceAdapter.h"

#include <memory>
#include <string>
#include <vector>

namespace etfdt::shell_services {

struct ShellAccountingStateSnapshot final {
    std::string actionName;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string dataQualityStatus = "UNAVAILABLE";
    std::string payloadStatus;
    bool hasRows = false;
    bool stale = false;
    std::vector<ShellAccountingIssue> issues;
};

class ShellAccountingReadOnlyController final {
public:
    ShellAccountingReadOnlyController() = default;

    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;
    [[nodiscard]] bool tradeDraftGenerationEnabled() const noexcept;
    [[nodiscard]] bool tradeSuggestionEnabled() const noexcept;
    [[nodiscard]] bool strategyExecutionEnabled() const noexcept;
    [[nodiscard]] bool brokerOrderEnabled() const noexcept;

    [[nodiscard]] ShellAccountingViewState currentState() const noexcept;
    [[nodiscard]] const std::vector<ShellAccountingIssue>& issues() const noexcept;
    [[nodiscard]] const std::string& currentActionName() const noexcept;

    void setPrivacyMode(bool enabled) noexcept;
    [[nodiscard]] bool privacyMode() const noexcept;
    [[nodiscard]] std::string displayText(const ShellAccountingDisplayText& text) const;

    void applyStateSnapshot(ShellAccountingStateSnapshot snapshot);
    void beginRefresh(std::string actionName);
    void markUnavailable(std::string actionName, ShellAccountingIssue issue);
    void setServiceAdapter(std::shared_ptr<ShellAccountingServiceAdapter> adapter) noexcept;
    [[nodiscard]] bool hasServiceAdapter() const noexcept;
    void clearServiceAdapter() noexcept;
    void refreshPositionList(const ShellAccountingServiceRequest& request);
    void refreshCashSummary(const ShellAccountingServiceRequest& request);
    void refreshPortfolioPnlSummary(const ShellAccountingServiceRequest& request);
    void refreshBasePositionSummary(const ShellAccountingServiceRequest& request);
    void refreshSniperPoolSummary(const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult createDraft(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult createDraftFromRecommendation(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult fetchTradeDraftReadOnlySummary(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult confirmDraft(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult submitManualTransaction(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult submitManualCashMovement(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult previewExcelVbaImportReadOnly(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult persistExcelVbaImportManualEntry(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult fetchPortfolioReplayReadOnlySummary(
        const ShellAccountingServiceRequest& request);
    [[nodiscard]] ShellAccountingServiceResult fetchStrategyRecommendationReadOnlySummary(
        const ShellAccountingServiceRequest& request);
    void reset();

private:
    void applyServiceResult(ShellAccountingServiceResult result);
    void markServiceAdapterNotConfigured(std::string actionName);

    std::string actionName_;
    ShellAccountingViewState state_ = ShellAccountingViewState::Idle;
    std::vector<ShellAccountingIssue> issues_;
    std::shared_ptr<ShellAccountingServiceAdapter> serviceAdapter_;
    bool privacyMode_ = false;
};

}  // namespace etfdt::shell_services
