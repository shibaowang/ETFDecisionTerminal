#include "FakeAccountingClientScenario.h"

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

}  // namespace

int main()
{
    const auto scenarios = etfdt::tests::shell_accounting_fake::stateTransitionScenarios();
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "implemented_false_to_unavailable"), "implemented=false scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "loading_to_loaded"), "Loaded scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "loading_to_empty"), "Empty scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "loading_to_warning"), "Warning scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "loading_to_degraded"), "DEGRADED scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "loading_to_error"), "Error scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "loading_to_stale"), "Stale scenario exists");

    for (const auto& scenario : scenarios) {
        etfdt::shell_services::ShellAccountingReadOnlyController controller;
        expectTrue(controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Idle, "controller starts Idle");
        controller.beginRefresh(scenario.payload.actionName);
        expectTrue(controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Loading, "beginRefresh transitions to Loading");
        controller.applyStateSnapshot(etfdt::tests::shell_accounting_fake::toStateSnapshot(scenario.payload));
        expectTrue(controller.currentState() == scenario.expectedState, "fake payload transitions to expected state");
        expectTrue(controller.readOnly(), "state transition keeps readOnly=true");
        expectTrue(!controller.writeEnabled(), "state transition keeps writeEnabled=false");
    }

    return gFailures == 0 ? 0 : 1;
}
