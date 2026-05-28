#include "FakeShellAccountingServiceAdapter.h"

#include <iostream>
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

void expectAction(
    etfdt::tests::shell_accounting_service_adapter::FakeShellAccountingServiceAdapter& adapter,
    const etfdt::shell_services::ShellAccountingServiceResult& result,
    std::string_view expectedAction)
{
    expectTrue(result.actionName == expectedAction, "fetch method returns expected action");
    expectTrue(adapter.lastActionName() == expectedAction, "fake adapter records expected action");
    expectTrue(result.readOnly, "fake adapter result remains read-only");
    expectTrue(!result.writeEnabled, "fake adapter result remains write-disabled");
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_service_adapter;

    FakeShellAccountingServiceAdapter adapter(makeGuardUnavailableResult(
        "position.list",
        "POSITION_LIST_NOT_AVAILABLE"));

    etfdt::shell_services::ShellAccountingServiceAdapter* interface = &adapter;

    expectAction(adapter, interface->fetchPositionList(makeDemoServiceRequest("position.list")), "position.list");
    expectAction(adapter, interface->fetchCashSummary(makeDemoServiceRequest("cash.summary")), "cash.summary");
    expectAction(
        adapter,
        interface->fetchPortfolioPnlSummary(makeDemoServiceRequest("portfolio.pnl.summary")),
        "portfolio.pnl.summary");
    expectAction(
        adapter,
        interface->fetchBasePositionSummary(makeDemoServiceRequest("base_position.summary")),
        "base_position.summary");
    expectAction(
        adapter,
        interface->fetchSniperPoolSummary(makeDemoServiceRequest("sniper_pool.summary")),
        "sniper_pool.summary");

    expectTrue(adapter.callCount() == 5, "fake adapter methods are called explicitly by tests only");

    return gFailures == 0 ? 0 : 1;
}
