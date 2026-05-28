#include "ShellServices/ShellAccountingState.h"

#include <array>

namespace etfdt::shell_services {

const char* toString(ShellAccountingViewState state) noexcept
{
    switch (state) {
    case ShellAccountingViewState::Idle:
        return "Idle";
    case ShellAccountingViewState::Loading:
        return "Loading";
    case ShellAccountingViewState::Loaded:
        return "Loaded";
    case ShellAccountingViewState::Empty:
        return "Empty";
    case ShellAccountingViewState::Unavailable:
        return "Unavailable";
    case ShellAccountingViewState::Warning:
        return "Warning";
    case ShellAccountingViewState::Error:
        return "Error";
    case ShellAccountingViewState::Stale:
        return "Stale";
    }
    return "Error";
}

ShellAccountingViewState shellAccountingViewStateFromDataQuality(
    const std::string& dataQualityStatus,
    bool hasRows)
{
    if (dataQualityStatus == "IDLE") {
        return ShellAccountingViewState::Idle;
    }
    if (dataQualityStatus == "LOADING") {
        return ShellAccountingViewState::Loading;
    }
    if (dataQualityStatus == "OK") {
        return hasRows ? ShellAccountingViewState::Loaded : ShellAccountingViewState::Empty;
    }
    if (dataQualityStatus == "WARNING" || dataQualityStatus == "DEGRADED") {
        return ShellAccountingViewState::Warning;
    }
    if (dataQualityStatus == "ERROR") {
        return ShellAccountingViewState::Error;
    }
    if (dataQualityStatus == "UNAVAILABLE") {
        return ShellAccountingViewState::Unavailable;
    }
    if (dataQualityStatus == "STALE") {
        return ShellAccountingViewState::Stale;
    }
    return ShellAccountingViewState::Error;
}

bool isShellAccountingGuardUnavailableStatus(const std::string& payloadStatus)
{
    constexpr std::array<const char*, 5> kStatuses{
        "POSITION_LIST_NOT_AVAILABLE",
        "CASH_SUMMARY_NOT_AVAILABLE",
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE",
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE",
    };

    for (const auto* status : kStatuses) {
        if (payloadStatus == status) {
            return true;
        }
    }
    return false;
}

}  // namespace etfdt::shell_services
