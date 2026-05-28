#pragma once

#include "ShellServices/ShellAccountingIssue.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellAccountingState.h"

#include <cstddef>
#include <string>
#include <vector>

namespace etfdt::shell_services {

class ShellAccountingStatusObject final {
public:
    ShellAccountingStatusObject() = default;

    void reset();
    void applyStateSnapshot(ShellAccountingStateSnapshot snapshot);

    [[nodiscard]] const std::string& actionName() const noexcept;
    [[nodiscard]] ShellAccountingViewState state() const noexcept;
    [[nodiscard]] const char* stateText() const noexcept;
    [[nodiscard]] bool implemented() const noexcept;
    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;
    [[nodiscard]] bool loading() const noexcept;
    [[nodiscard]] bool unavailable() const noexcept;
    [[nodiscard]] bool warning() const noexcept;
    [[nodiscard]] bool error() const noexcept;
    [[nodiscard]] bool stale() const noexcept;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] const std::string& dataQualityStatus() const noexcept;
    [[nodiscard]] const std::string& payloadStatus() const noexcept;
    [[nodiscard]] std::size_t issueCount() const noexcept;
    [[nodiscard]] std::size_t blockingIssueCount() const noexcept;

private:
    std::string actionName_;
    ShellAccountingViewState state_ = ShellAccountingViewState::Idle;
    bool implemented_ = false;
    bool readOnly_ = true;
    bool writeEnabled_ = false;
    std::string dataQualityStatus_ = "IDLE";
    std::string payloadStatus_;
    std::vector<ShellAccountingIssue> issues_;
};

}  // namespace etfdt::shell_services
