#pragma once

#include "ShellServices/ShellAccountingState.h"

#include <string>

namespace etfdt::shell_services {

struct ShellAccountingDisplayText final {
    std::string rawText;
    std::string maskedText;
};

[[nodiscard]] std::string shellAccountingDisplayText(
    const ShellAccountingDisplayText& text,
    bool privacyMode,
    ShellAccountingViewState viewState);

}  // namespace etfdt::shell_services
