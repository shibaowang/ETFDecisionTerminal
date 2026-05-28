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

void expectUnavailable(
    const etfdt::shell_services::ShellAccountingReadOnlyController& controller,
    std::string_view message)
{
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        message);
    expectTrue(controller.readOnly(), "unavailable path remains readOnly=true");
    expectTrue(!controller.writeEnabled(), "unavailable path remains writeEnabled=false");
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_service_adapter;

    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectUnavailable(controller, "missing adapter maps to Unavailable");
    expectTrue(!controller.issues().empty(), "missing adapter issue is visible");
    expectTrue(
        controller.issues().front().code == "SERVICE_ADAPTER_NOT_CONFIGURED",
        "missing adapter issue code is retained");

    auto fakeAdapter = std::make_shared<FakeShellAccountingServiceAdapter>();
    controller.setServiceAdapter(fakeAdapter);

    fakeAdapter->setNextResult(
        makeGuardUnavailableResult("position.list", "POSITION_LIST_NOT_AVAILABLE"));
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectUnavailable(controller, "position.list guard payload maps to Unavailable");

    fakeAdapter->setNextResult(makeGuardUnavailableResult("cash.summary", "CASH_SUMMARY_NOT_AVAILABLE"));
    controller.refreshCashSummary(makeDemoServiceRequest("cash.summary"));
    expectUnavailable(controller, "cash.summary guard payload maps to Unavailable");

    fakeAdapter->setNextResult(makeGuardUnavailableResult(
        "portfolio.pnl.summary",
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"));
    controller.refreshPortfolioPnlSummary(makeDemoServiceRequest("portfolio.pnl.summary"));
    expectUnavailable(controller, "portfolio.pnl.summary guard payload maps to Unavailable");

    fakeAdapter->setNextResult(makeGuardUnavailableResult(
        "base_position.summary",
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE"));
    controller.refreshBasePositionSummary(makeDemoServiceRequest("base_position.summary"));
    expectUnavailable(controller, "base_position.summary guard payload maps to Unavailable");

    fakeAdapter->setNextResult(makeGuardUnavailableResult(
        "sniper_pool.summary",
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"));
    controller.refreshSniperPoolSummary(makeDemoServiceRequest("sniper_pool.summary"));
    expectUnavailable(controller, "sniper_pool.summary guard payload maps to Unavailable");

    etfdt::shell_services::ShellAccountingServiceResult emptyResult;
    emptyResult.protocolSuccess = true;
    emptyResult.implemented = true;
    emptyResult.readOnly = true;
    emptyResult.writeEnabled = false;
    emptyResult.dataQualityStatus = "OK";
    emptyResult.hasRows = false;
    fakeAdapter->setNextResult(emptyResult);
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Empty,
        "Empty stays distinct from Unavailable");

    return gFailures == 0 ? 0 : 1;
}
