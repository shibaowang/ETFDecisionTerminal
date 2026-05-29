#include "ShellServices/ShellAccountingStatusObject.h"

#include <algorithm>
#include <utility>

namespace etfdt::shell_services {

ShellAccountingStatusObject::ShellAccountingStatusObject(QObject* parent)
    : QObject(parent)
{
}

void ShellAccountingStatusObject::reset()
{
    actionName_.clear();
    state_ = ShellAccountingViewState::Idle;
    implemented_ = false;
    readOnly_ = true;
    writeEnabled_ = false;
    dataQualityStatus_ = "IDLE";
    payloadStatus_.clear();
    issues_.clear();
}

void ShellAccountingStatusObject::applyStateSnapshot(ShellAccountingStateSnapshot snapshot)
{
    actionName_ = std::move(snapshot.actionName);
    implemented_ = snapshot.implemented;
    readOnly_ = true;
    writeEnabled_ = false;
    dataQualityStatus_ = std::move(snapshot.dataQualityStatus);
    payloadStatus_ = std::move(snapshot.payloadStatus);
    issues_ = std::move(snapshot.issues);

    if (!implemented_ || isShellAccountingGuardUnavailableStatus(payloadStatus_)) {
        state_ = ShellAccountingViewState::Unavailable;
        return;
    }

    if (snapshot.stale) {
        state_ = ShellAccountingViewState::Stale;
        return;
    }

    state_ = shellAccountingViewStateFromDataQuality(dataQualityStatus_, snapshot.hasRows);
}

const std::string& ShellAccountingStatusObject::actionName() const noexcept
{
    return actionName_;
}

QString ShellAccountingStatusObject::actionNameText() const
{
    return QString::fromStdString(actionName_);
}

ShellAccountingViewState ShellAccountingStatusObject::state() const noexcept
{
    return state_;
}

const char* ShellAccountingStatusObject::stateText() const noexcept
{
    return toString(state_);
}

QString ShellAccountingStatusObject::stateTextQml() const
{
    return QString::fromUtf8(stateText());
}

bool ShellAccountingStatusObject::implemented() const noexcept
{
    return implemented_;
}

bool ShellAccountingStatusObject::readOnly() const noexcept
{
    return readOnly_;
}

bool ShellAccountingStatusObject::writeEnabled() const noexcept
{
    return writeEnabled_;
}

bool ShellAccountingStatusObject::loading() const noexcept
{
    return state_ == ShellAccountingViewState::Loading;
}

bool ShellAccountingStatusObject::unavailable() const noexcept
{
    return state_ == ShellAccountingViewState::Unavailable;
}

bool ShellAccountingStatusObject::warning() const noexcept
{
    return state_ == ShellAccountingViewState::Warning;
}

bool ShellAccountingStatusObject::error() const noexcept
{
    return state_ == ShellAccountingViewState::Error;
}

bool ShellAccountingStatusObject::stale() const noexcept
{
    return state_ == ShellAccountingViewState::Stale;
}

bool ShellAccountingStatusObject::empty() const noexcept
{
    return state_ == ShellAccountingViewState::Empty;
}

const std::string& ShellAccountingStatusObject::dataQualityStatus() const noexcept
{
    return dataQualityStatus_;
}

const std::string& ShellAccountingStatusObject::payloadStatus() const noexcept
{
    return payloadStatus_;
}

std::size_t ShellAccountingStatusObject::issueCount() const noexcept
{
    return issues_.size();
}

std::size_t ShellAccountingStatusObject::blockingIssueCount() const noexcept
{
    return static_cast<std::size_t>(std::count_if(
        issues_.begin(),
        issues_.end(),
        [](const ShellAccountingIssue& issue) { return issue.blocking; }));
}

}  // namespace etfdt::shell_services
