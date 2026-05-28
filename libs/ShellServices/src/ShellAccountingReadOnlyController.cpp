#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <utility>

namespace etfdt::shell_services {

bool ShellAccountingReadOnlyController::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingReadOnlyController::writeEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::tradeDraftGenerationEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::tradeSuggestionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::strategyExecutionEnabled() const noexcept
{
    return false;
}

bool ShellAccountingReadOnlyController::brokerOrderEnabled() const noexcept
{
    return false;
}

ShellAccountingViewState ShellAccountingReadOnlyController::currentState() const noexcept
{
    return state_;
}

const std::vector<ShellAccountingIssue>& ShellAccountingReadOnlyController::issues() const noexcept
{
    return issues_;
}

const std::string& ShellAccountingReadOnlyController::currentActionName() const noexcept
{
    return actionName_;
}

void ShellAccountingReadOnlyController::setPrivacyMode(bool enabled) noexcept
{
    privacyMode_ = enabled;
}

bool ShellAccountingReadOnlyController::privacyMode() const noexcept
{
    return privacyMode_;
}

std::string ShellAccountingReadOnlyController::displayText(
    const ShellAccountingDisplayText& text) const
{
    return shellAccountingDisplayText(text, privacyMode_, state_);
}

void ShellAccountingReadOnlyController::applyStateSnapshot(
    ShellAccountingStateSnapshot snapshot)
{
    actionName_ = std::move(snapshot.actionName);
    issues_ = std::move(snapshot.issues);

    if (!snapshot.implemented ||
        isShellAccountingGuardUnavailableStatus(snapshot.payloadStatus)) {
        state_ = ShellAccountingViewState::Unavailable;
        return;
    }

    if (snapshot.stale) {
        state_ = ShellAccountingViewState::Stale;
        return;
    }

    state_ = shellAccountingViewStateFromDataQuality(
        snapshot.dataQualityStatus,
        snapshot.hasRows);
}

void ShellAccountingReadOnlyController::beginRefresh(std::string actionName)
{
    actionName_ = std::move(actionName);
    state_ = ShellAccountingViewState::Loading;
    issues_.clear();
}

void ShellAccountingReadOnlyController::markUnavailable(
    std::string actionName,
    ShellAccountingIssue issue)
{
    actionName_ = std::move(actionName);
    state_ = ShellAccountingViewState::Unavailable;
    issues_.clear();
    issues_.push_back(std::move(issue));
}

void ShellAccountingReadOnlyController::setServiceAdapter(
    std::shared_ptr<ShellAccountingServiceAdapter> adapter) noexcept
{
    serviceAdapter_ = std::move(adapter);
}

bool ShellAccountingReadOnlyController::hasServiceAdapter() const noexcept
{
    return static_cast<bool>(serviceAdapter_);
}

void ShellAccountingReadOnlyController::clearServiceAdapter() noexcept
{
    serviceAdapter_.reset();
}

void ShellAccountingReadOnlyController::reset()
{
    actionName_.clear();
    state_ = ShellAccountingViewState::Idle;
    issues_.clear();
    privacyMode_ = false;
    serviceAdapter_.reset();
}

}  // namespace etfdt::shell_services
