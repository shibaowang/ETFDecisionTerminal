#include "ShellAccountingControllerConcretePortIntegrationTestUtils.h"

#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"

#include <algorithm>
#include <memory>
#include <utility>

namespace etfdt::shell_services::tests {

ShellAccountingServiceRequest makeControllerConcretePortRequest(
    std::string actionName)
{
    ShellAccountingServiceRequest request;
    request.actionName = std::move(actionName);
    request.accountId = "ACC-CONTROLLER-CONCRETE-PORT";
    request.portfolioId = "PF-CONTROLLER-CONCRETE-PORT";
    request.includeIssues = true;
    request.includeDebugMetadata = false;
    request.requestedOutputs = {"guard"};
    request.timeoutMs = 200;
    return request;
}

ControllerConcretePortGuardResult refreshControllerThroughConcretePort(
    FakeAccountingWrapperServer& server,
    const std::string& actionName)
{
    ControllerConcretePortGuardResult result;
    auto client = makeConnectedClient(server);
    result.connected = static_cast<bool>(client);
    if (!client) {
        return result;
    }

    auto concretePort = std::make_shared<ShellAccountingDataServiceClientPortAdapter>(client);
    auto dataServiceAdapter = std::make_shared<ShellAccountingDataServiceAdapter>(concretePort);

    ShellAccountingReadOnlyController controller;
    controller.setServiceAdapter(dataServiceAdapter);

    const auto request = makeControllerConcretePortRequest(actionName);
    if (actionName == "position.list") {
        controller.refreshPositionList(request);
    }
    else if (actionName == "cash.summary") {
        controller.refreshCashSummary(request);
    }
    else if (actionName == "portfolio.pnl.summary") {
        controller.refreshPortfolioPnlSummary(request);
    }
    else if (actionName == "base_position.summary") {
        controller.refreshBasePositionSummary(request);
    }
    else if (actionName == "sniper_pool.summary") {
        controller.refreshSniperPoolSummary(request);
    }

    result.refreshed = server.waitForActionCount(1);
    result.observedAction = server.actions().empty() ? std::string{} : server.actions().front();
    result.state = controller.currentState();
    result.issues = controller.issues();
    result.controllerReadOnly = controller.readOnly();
    result.controllerWriteEnabled = controller.writeEnabled();
    result.tradeDraftEnabled = controller.tradeDraftGenerationEnabled();
    result.tradeSuggestionEnabled = controller.tradeSuggestionEnabled();
    result.strategyExecutionEnabled = controller.strategyExecutionEnabled();
    result.brokerOrderEnabled = controller.brokerOrderEnabled();
    return result;
}

bool controllerIssuesContainCode(
    const std::vector<ShellAccountingIssue>& issues,
    const std::string& code)
{
    return std::any_of(issues.begin(), issues.end(), [&](const ShellAccountingIssue& issue) {
        return issue.code == code;
    });
}

}  // namespace etfdt::shell_services::tests
