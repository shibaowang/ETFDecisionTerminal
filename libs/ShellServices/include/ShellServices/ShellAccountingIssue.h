#pragma once

#include "ShellServices/ShellAccountingState.h"

#include <string>

namespace etfdt::shell_services {

struct ShellAccountingIssue final {
    std::string code;
    std::string level;
    std::string message;
    bool blocking = false;
    std::string sourceId;
};

[[nodiscard]] ShellAccountingViewState shellAccountingViewStateFromIssue(
    const ShellAccountingIssue& issue);

}  // namespace etfdt::shell_services
