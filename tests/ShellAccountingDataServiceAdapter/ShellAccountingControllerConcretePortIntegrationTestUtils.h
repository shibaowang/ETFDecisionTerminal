#pragma once

#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellAccountingDataServiceClientPortAdapterTestUtils.h"

#include <memory>
#include <string>

namespace etfdt::shell_services::tests {

struct ControllerConcretePortGuardResult final {
    bool connected = false;
    bool refreshed = false;
    std::string observedAction;
    ShellAccountingViewState state = ShellAccountingViewState::Idle;
    std::vector<ShellAccountingIssue> issues;
    bool controllerReadOnly = false;
    bool controllerWriteEnabled = true;
    bool tradeDraftEnabled = true;
    bool tradeSuggestionEnabled = true;
    bool strategyExecutionEnabled = true;
    bool brokerOrderEnabled = true;
};

[[nodiscard]] ShellAccountingServiceRequest makeControllerConcretePortRequest(
    std::string actionName);

[[nodiscard]] ControllerConcretePortGuardResult refreshControllerThroughConcretePort(
    FakeAccountingWrapperServer& server,
    const std::string& actionName);

[[nodiscard]] bool controllerIssuesContainCode(
    const std::vector<ShellAccountingIssue>& issues,
    const std::string& code);

}  // namespace etfdt::shell_services::tests
