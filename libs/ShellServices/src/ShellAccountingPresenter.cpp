#include "ShellServices/ShellAccountingPresenter.h"

#include <string>
#include <utility>
#include <vector>

namespace etfdt::shell_services {

namespace {

ShellAccountingIssue makePresenterIssue(
    std::string code,
    std::string message,
    bool blocking = true)
{
    return {
        std::move(code),
        blocking ? "ERROR" : "WARNING",
        std::move(message),
        blocking,
        "ShellAccountingPresenter",
    };
}

ShellAccountingStateSnapshot snapshotFromControllerState(
    std::string actionName,
    ShellAccountingViewState state,
    std::vector<ShellAccountingIssue> issues)
{
    ShellAccountingStateSnapshot snapshot;
    snapshot.actionName = std::move(actionName);
    snapshot.readOnly = true;
    snapshot.writeEnabled = false;
    snapshot.issues = std::move(issues);

    switch (state) {
    case ShellAccountingViewState::Idle:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "IDLE";
        break;
    case ShellAccountingViewState::Loading:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "LOADING";
        break;
    case ShellAccountingViewState::Loaded:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "OK";
        snapshot.hasRows = true;
        break;
    case ShellAccountingViewState::Empty:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "OK";
        snapshot.hasRows = false;
        break;
    case ShellAccountingViewState::Unavailable:
        snapshot.implemented = false;
        snapshot.dataQualityStatus = "UNAVAILABLE";
        snapshot.payloadStatus = "SHELL_ACCOUNTING_CONTROLLER_UNAVAILABLE";
        break;
    case ShellAccountingViewState::Warning:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "WARNING";
        snapshot.hasRows = true;
        break;
    case ShellAccountingViewState::Error:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "ERROR";
        break;
    case ShellAccountingViewState::Stale:
        snapshot.implemented = true;
        snapshot.dataQualityStatus = "STALE";
        snapshot.stale = true;
        snapshot.hasRows = true;
        break;
    }

    return snapshot;
}

}  // namespace

ShellAccountingPresenter::ShellAccountingPresenter() = default;

bool ShellAccountingPresenter::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingPresenter::writeEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::tradeDraftGenerationEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::tradeSuggestionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::strategyExecutionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingPresenter::brokerSubmissionEnabled() const noexcept
{
    return false;
}

ShellAccountingStatusObject& ShellAccountingPresenter::statusObject() noexcept
{
    return status_;
}

const ShellAccountingStatusObject& ShellAccountingPresenter::statusObject() const noexcept
{
    return status_;
}

ShellAccountingIssueListModel& ShellAccountingPresenter::issueListModel() noexcept
{
    return issues_;
}

const ShellAccountingIssueListModel& ShellAccountingPresenter::issueListModel() const noexcept
{
    return issues_;
}

ShellPositionListModel& ShellAccountingPresenter::positionListModel() noexcept
{
    return positions_;
}

const ShellPositionListModel& ShellAccountingPresenter::positionListModel() const noexcept
{
    return positions_;
}

void ShellAccountingPresenter::setController(
    std::shared_ptr<ShellAccountingReadOnlyController> controller)
{
    controller_ = std::move(controller);
    if (controller_) {
        controller_->setPrivacyMode(privacyMode_);
        syncFromController();
    }
}

void ShellAccountingPresenter::clearController() noexcept
{
    controller_.reset();
}

bool ShellAccountingPresenter::hasController() const noexcept
{
    return static_cast<bool>(controller_);
}

void ShellAccountingPresenter::setPrivacyMode(bool enabled)
{
    privacyMode_ = enabled;
    positions_.setPrivacyMode(enabled);
    if (controller_) {
        controller_->setPrivacyMode(enabled);
    }
}

bool ShellAccountingPresenter::privacyMode() const noexcept
{
    return privacyMode_;
}

void ShellAccountingPresenter::syncFromController()
{
    if (!controller_) {
        return;
    }

    applyControllerState(
        controller_->currentActionName(),
        controller_->currentState(),
        controller_->issues());
}

void ShellAccountingPresenter::refreshPositionList()
{
    if (!controller_) {
        markControllerNotConfigured("position.list");
        return;
    }

    ShellAccountingServiceRequest request;
    request.actionName = "position.list";
    controller_->refreshPositionList(request);
    syncFromController();
}

void ShellAccountingPresenter::refreshAllReadOnly()
{
    refreshPositionList();
}

void ShellAccountingPresenter::reset()
{
    controller_.reset();
    privacyMode_ = false;
    status_.reset();
    issues_.clearIssues();
    positions_.clearRows();
    positions_.setPrivacyMode(false);
}

void ShellAccountingPresenter::markControllerNotConfigured(const char* actionName)
{
    std::vector<ShellAccountingIssue> issues{
        makePresenterIssue(
            "CONTROLLER_NOT_CONFIGURED",
            "Shell accounting controller is not configured."),
    };
    applyControllerState(actionName, ShellAccountingViewState::Unavailable, std::move(issues));
    positions_.clearRows();
}

void ShellAccountingPresenter::applyControllerState(
    const std::string& actionName,
    ShellAccountingViewState state,
    std::vector<ShellAccountingIssue> issues)
{
    auto issueRows = issues;
    status_.applyStateSnapshot(
        snapshotFromControllerState(actionName, state, std::move(issues)));
    issues_.setIssues(std::move(issueRows));
}

}  // namespace etfdt::shell_services
