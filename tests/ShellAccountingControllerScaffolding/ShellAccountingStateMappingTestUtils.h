#pragma once

#include "ShellAccountingControllerScenario.h"

#include <string>

namespace etfdt::tests::shell_accounting {

[[nodiscard]] std::string mapScenarioToViewState(const ShellAccountingControllerScenario& scenario);
[[nodiscard]] std::string mapIssueToViewState(const ShellAccountingIssueDescriptor& issue);
[[nodiscard]] std::string displayTextForPrivacy(
    const std::string& rawText,
    const std::string& maskedText,
    bool privacyMode,
    const std::string& viewState);
[[nodiscard]] bool preservesReadOnlyBoundary(const ShellAccountingControllerScenario& scenario);
[[nodiscard]] bool hidesTradeActions(const ShellAccountingTradeActionBoundary& boundary);

}  // namespace etfdt::tests::shell_accounting
