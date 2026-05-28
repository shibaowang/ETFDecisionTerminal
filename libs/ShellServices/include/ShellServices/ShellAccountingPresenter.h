#pragma once

#include "ShellServices/ShellAccountingIssueListModel.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellAccountingStatusObject.h"
#include "ShellServices/ShellPositionListModel.h"

#include <memory>

namespace etfdt::shell_services {

class ShellAccountingPresenter final {
public:
    ShellAccountingPresenter();

    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;
    [[nodiscard]] bool tradeDraftGenerationEnabled() const noexcept;
    [[nodiscard]] bool tradeSuggestionEnabled() const noexcept;
    [[nodiscard]] bool strategyExecutionEnabled() const noexcept;
    [[nodiscard]] bool brokerSubmissionEnabled() const noexcept;

    [[nodiscard]] ShellAccountingStatusObject& statusObject() noexcept;
    [[nodiscard]] const ShellAccountingStatusObject& statusObject() const noexcept;
    [[nodiscard]] ShellAccountingIssueListModel& issueListModel() noexcept;
    [[nodiscard]] const ShellAccountingIssueListModel& issueListModel() const noexcept;
    [[nodiscard]] ShellPositionListModel& positionListModel() noexcept;
    [[nodiscard]] const ShellPositionListModel& positionListModel() const noexcept;

    void setController(std::shared_ptr<ShellAccountingReadOnlyController> controller);
    void clearController() noexcept;
    [[nodiscard]] bool hasController() const noexcept;

    void setPrivacyMode(bool enabled);
    [[nodiscard]] bool privacyMode() const noexcept;

    void syncFromController();
    void refreshPositionList();
    void refreshCashSummary();
    void refreshPortfolioPnlSummary();
    void refreshBasePositionSummary();
    void refreshSniperPoolSummary();
    void refreshAllReadOnly();
    void reset();

private:
    void markControllerNotConfigured(const char* actionName);
    void refreshAccountingAction(
        const char* actionName,
        void (ShellAccountingReadOnlyController::*refresh)(
            const ShellAccountingServiceRequest&));
    void applyControllerState(
        const std::string& actionName,
        ShellAccountingViewState state,
        std::vector<ShellAccountingIssue> issues);

    ShellAccountingStatusObject status_;
    ShellAccountingIssueListModel issues_;
    ShellPositionListModel positions_;
    std::shared_ptr<ShellAccountingReadOnlyController> controller_;
    bool privacyMode_ = false;
};

}  // namespace etfdt::shell_services
