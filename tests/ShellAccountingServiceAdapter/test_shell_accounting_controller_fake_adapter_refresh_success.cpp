#include "FakeShellAccountingServiceAdapter.h"

#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <iostream>
#include <memory>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

etfdt::shell_services::ShellAccountingServiceResult okResult(bool hasRows)
{
    etfdt::shell_services::ShellAccountingServiceResult result;
    result.protocolSuccess = true;
    result.implemented = true;
    result.readOnly = true;
    result.writeEnabled = false;
    result.dataQualityStatus = "OK";
    result.hasRows = hasRows;
    return result;
}

void expectReadOnlyBoundary(const etfdt::shell_services::ShellAccountingReadOnlyController& controller)
{
    expectTrue(controller.readOnly(), "controller remains readOnly=true");
    expectTrue(!controller.writeEnabled(), "controller remains writeEnabled=false");
    expectTrue(!controller.tradeDraftGenerationEnabled(), "controller does not enable TradeDraft");
    expectTrue(!controller.tradeSuggestionEnabled(), "controller does not enable trade suggestion");
    expectTrue(!controller.strategyExecutionEnabled(), "controller does not enable strategy execution");
    expectTrue(!controller.brokerOrderEnabled(), "controller does not enable broker order");
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_service_adapter;

    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    auto fakeAdapter = std::make_shared<FakeShellAccountingServiceAdapter>(okResult(true));
    controller.setServiceAdapter(fakeAdapter);
    expectTrue(controller.hasServiceAdapter(), "controller has injected fake adapter");

    bool observedLoading = false;
    fakeAdapter->setOnFetch([&controller, &observedLoading]() {
        observedLoading =
            controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Loading;
    });

    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectTrue(observedLoading, "refreshPositionList enters Loading before adapter fetch");
    expectTrue(fakeAdapter->callCount() == 1, "refreshPositionList calls fake fetchPositionList");
    expectTrue(fakeAdapter->lastActionName() == "position.list", "position.list action is called");
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Loaded,
        "OK with rows maps to Loaded");
    expectReadOnlyBoundary(controller);

    fakeAdapter->setOnFetch({});
    controller.refreshCashSummary(makeDemoServiceRequest("cash.summary"));
    expectTrue(fakeAdapter->lastActionName() == "cash.summary", "cash.summary action is called");
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Loaded,
        "cash.summary OK with rows maps to Loaded");

    controller.refreshPortfolioPnlSummary(makeDemoServiceRequest("portfolio.pnl.summary"));
    expectTrue(
        fakeAdapter->lastActionName() == "portfolio.pnl.summary",
        "portfolio.pnl.summary action is called");

    controller.refreshBasePositionSummary(makeDemoServiceRequest("base_position.summary"));
    expectTrue(
        fakeAdapter->lastActionName() == "base_position.summary",
        "base_position.summary action is called");

    controller.refreshSniperPoolSummary(makeDemoServiceRequest("sniper_pool.summary"));
    expectTrue(
        fakeAdapter->lastActionName() == "sniper_pool.summary",
        "sniper_pool.summary action is called");
    expectTrue(fakeAdapter->callCount() == 5, "all five refresh methods called fake adapter");

    fakeAdapter->setNextResult(okResult(false));
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Empty,
        "OK without rows maps to Empty");
    expectReadOnlyBoundary(controller);

    return gFailures == 0 ? 0 : 1;
}
