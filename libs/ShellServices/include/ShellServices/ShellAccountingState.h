#pragma once

#include <string>

namespace etfdt::shell_services {

enum class ShellAccountingViewState {
    Idle,
    Loading,
    Loaded,
    Empty,
    Unavailable,
    Warning,
    Error,
    Stale,
};

[[nodiscard]] const char* toString(ShellAccountingViewState state) noexcept;
[[nodiscard]] ShellAccountingViewState shellAccountingViewStateFromDataQuality(
    const std::string& dataQualityStatus,
    bool hasRows);
[[nodiscard]] bool isShellAccountingGuardUnavailableStatus(const std::string& payloadStatus);

}  // namespace etfdt::shell_services
