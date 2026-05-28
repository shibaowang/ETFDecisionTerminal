#pragma once

#include "ShellAccountingControllerConcretePortIntegrationTestUtils.h"

#include "ShellServices/ShellAccountingPresenter.h"

#include <memory>
#include <string>
#include <vector>

namespace etfdt::shell_services::tests {

struct PresenterConcretePortGuardResult final {
    bool connected = false;
    bool refreshed = false;
    std::string observedAction;
    ShellAccountingViewState state = ShellAccountingViewState::Idle;
    std::vector<ShellAccountingIssue> issues;
    bool presenterReadOnly = false;
    bool presenterWriteEnabled = true;
    bool tradeDraftEnabled = true;
    bool tradeSuggestionEnabled = true;
    bool strategyExecutionEnabled = true;
    bool brokerSubmissionEnabled = true;
    bool presenterHasController = false;
    int positionRowCount = -1;
};

[[nodiscard]] PresenterConcretePortGuardResult refreshPresenterPositionListThroughConcretePort(
    FakeAccountingWrapperServer& server,
    bool useRefreshAllReadOnly = false,
    bool privacyMode = false);

[[nodiscard]] bool presenterIssuesContainCode(
    const std::vector<ShellAccountingIssue>& issues,
    const std::string& code);

[[nodiscard]] bool containsPresenterForbiddenActionText(const std::string& action);

}  // namespace etfdt::shell_services::tests
