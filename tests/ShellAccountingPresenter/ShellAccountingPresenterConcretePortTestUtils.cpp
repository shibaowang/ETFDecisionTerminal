#include "ShellAccountingPresenterConcretePortTestUtils.h"

#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <algorithm>
#include <memory>

namespace etfdt::shell_services::tests {

namespace {

void refreshPresenterAction(ShellAccountingPresenter& presenter, const std::string& actionName)
{
    if (actionName == "position.list") {
        presenter.refreshPositionList();
    }
    else if (actionName == "cash.summary") {
        presenter.refreshCashSummary();
    }
    else if (actionName == "portfolio.pnl.summary") {
        presenter.refreshPortfolioPnlSummary();
    }
    else if (actionName == "base_position.summary") {
        presenter.refreshBasePositionSummary();
    }
    else if (actionName == "sniper_pool.summary") {
        presenter.refreshSniperPoolSummary();
    }
}

PresenterConcretePortGuardResult refreshPresenterThroughConcretePort(
    FakeAccountingWrapperServer& server,
    const std::string& actionName,
    bool useRefreshAllReadOnly,
    bool privacyMode,
    std::size_t expectedActionCount)
{
    PresenterConcretePortGuardResult result;
    auto client = makeConnectedClient(server);
    result.connected = static_cast<bool>(client);
    if (!client) {
        return result;
    }

    auto concretePort = std::make_shared<ShellAccountingDataServiceClientPortAdapter>(client);
    auto dataServiceAdapter = std::make_shared<ShellAccountingDataServiceAdapter>(concretePort);
    auto controller = std::make_shared<ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(dataServiceAdapter);

    ShellAccountingPresenter presenter;
    presenter.setController(controller);
    presenter.setPrivacyMode(privacyMode);

    if (useRefreshAllReadOnly) {
        presenter.refreshAllReadOnly();
    }
    else {
        refreshPresenterAction(presenter, actionName);
    }

    result.refreshed = server.waitForActionCount(expectedActionCount);
    result.observedAction = server.actions().empty() ? std::string{} : server.actions().front();
    result.observedActions = server.actions();
    result.state = presenter.statusObject().state();
    result.issues = presenter.issueListModel().issues();
    result.presenterReadOnly = presenter.readOnly();
    result.presenterWriteEnabled = presenter.writeEnabled();
    result.tradeDraftEnabled = presenter.tradeDraftGenerationEnabled();
    result.tradeSuggestionEnabled = presenter.tradeSuggestionEnabled();
    result.strategyExecutionEnabled = presenter.strategyExecutionEnabled();
    result.brokerSubmissionEnabled = presenter.brokerSubmissionEnabled();
    result.presenterHasController = presenter.hasController();
    result.positionRowCount = presenter.positionListModel().rowCount();
    return result;
}

}  // namespace

PresenterConcretePortGuardResult refreshPresenterPositionListThroughConcretePort(
    FakeAccountingWrapperServer& server,
    bool useRefreshAllReadOnly,
    bool privacyMode)
{
    return refreshPresenterThroughConcretePort(
        server,
        "position.list",
        useRefreshAllReadOnly,
        privacyMode,
        useRefreshAllReadOnly ? 5U : 1U);
}

PresenterConcretePortGuardResult refreshPresenterActionThroughConcretePort(
    FakeAccountingWrapperServer& server,
    const std::string& actionName,
    bool privacyMode)
{
    return refreshPresenterThroughConcretePort(
        server,
        actionName,
        false,
        privacyMode,
        1U);
}

PresenterConcretePortGuardResult refreshPresenterAllReadOnlyThroughConcretePort(
    FakeAccountingWrapperServer& server,
    bool privacyMode)
{
    return refreshPresenterThroughConcretePort(
        server,
        "accounting.refresh_all",
        true,
        privacyMode,
        5U);
}

bool presenterIssuesContainCode(
    const std::vector<ShellAccountingIssue>& issues,
    const std::string& code)
{
    return std::any_of(issues.begin(), issues.end(), [&](const ShellAccountingIssue& issue) {
        return issue.code == code;
    });
}

bool containsPresenterForbiddenActionText(const std::string& action)
{
    return action == "data.audit.append" ||
        action.find("trade") != std::string::npos ||
        action.find("draft") != std::string::npos ||
        action.find("snapshot") != std::string::npos ||
        action.find("strategy") != std::string::npos ||
        action.find("broker") != std::string::npos;
}

}  // namespace etfdt::shell_services::tests
