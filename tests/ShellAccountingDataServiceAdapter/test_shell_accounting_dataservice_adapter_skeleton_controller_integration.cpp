#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

etfdt::shell_services::ShellAccountingServiceRequest requestFor(std::string actionName)
{
    etfdt::shell_services::ShellAccountingServiceRequest request;
    request.actionName = std::move(actionName);
    request.includeIssues = true;
    return request;
}

void expectControllerUnavailable(
    const etfdt::shell_services::ShellAccountingReadOnlyController& controller,
    std::string_view expectedAction)
{
    expectTrue(controller.currentActionName() == expectedAction, "controller actionName is retained");
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "skeleton adapter result maps to Unavailable");
    expectTrue(controller.readOnly(), "controller remains readOnly=true");
    expectTrue(!controller.writeEnabled(), "controller remains writeEnabled=false");
    expectTrue(!controller.tradeDraftGenerationEnabled(), "controller does not enable TradeDraft");
    expectTrue(!controller.tradeSuggestionEnabled(), "controller does not enable trade suggestion");
    expectTrue(!controller.strategyExecutionEnabled(), "controller does not enable strategy");
    expectTrue(!controller.brokerOrderEnabled(), "controller does not enable broker order");
    expectTrue(!controller.issues().empty(), "controller exposes adapter not connected issue");
    if (!controller.issues().empty()) {
        expectTrue(
            controller.issues().front().code == "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED",
            "controller retains adapter issue code");
        expectTrue(controller.issues().front().blocking, "controller retains blocking issue");
    }
}

}  // namespace

int main()
{
    auto adapter = std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>();
    etfdt::shell_services::ShellAccountingReadOnlyController controller;

    expectTrue(!controller.hasServiceAdapter(), "controller starts without adapter");
    controller.setServiceAdapter(adapter);
    expectTrue(controller.hasServiceAdapter(), "controller accepts skeleton adapter injection");

    controller.refreshPositionList(requestFor("position.list"));
    expectControllerUnavailable(controller, "position.list");

    controller.refreshCashSummary(requestFor("cash.summary"));
    expectControllerUnavailable(controller, "cash.summary");

    controller.refreshPortfolioPnlSummary(requestFor("portfolio.pnl.summary"));
    expectControllerUnavailable(controller, "portfolio.pnl.summary");

    controller.refreshBasePositionSummary(requestFor("base_position.summary"));
    expectControllerUnavailable(controller, "base_position.summary");

    controller.refreshSniperPoolSummary(requestFor("sniper_pool.summary"));
    expectControllerUnavailable(controller, "sniper_pool.summary");

    return gFailures == 0 ? 0 : 1;
}
