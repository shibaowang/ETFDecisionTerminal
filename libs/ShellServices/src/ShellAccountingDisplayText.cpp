#include "ShellServices/ShellAccountingDisplayText.h"

namespace etfdt::shell_services {

std::string shellAccountingDisplayText(
    const ShellAccountingDisplayText& text,
    bool privacyMode,
    ShellAccountingViewState viewState)
{
    if (viewState == ShellAccountingViewState::Unavailable ||
        viewState == ShellAccountingViewState::Empty) {
        return {};
    }

    return privacyMode ? text.maskedText : text.rawText;
}

}  // namespace etfdt::shell_services
