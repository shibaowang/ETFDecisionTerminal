#include "ShellAccountingControllerScenario.h"
#include "ShellAccountingStateMappingTestUtils.h"

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
    constexpr std::string_view kContractName = "shell_accounting_controller_state_contract";
    expectTrue(!kContractName.empty(), "state contract CTest name is documented in test source");

    const auto guardScenarios = etfdt::tests::shell_accounting::guardPayloadScenarios();
    expectTrue(etfdt::tests::shell_accounting::hasActionScenario(guardScenarios, "position.list"), "position.list guard scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasActionScenario(guardScenarios, "cash.summary"), "cash.summary guard scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasActionScenario(guardScenarios, "portfolio.pnl.summary"), "portfolio.pnl.summary guard scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasActionScenario(guardScenarios, "base_position.summary"), "base_position.summary guard scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasActionScenario(guardScenarios, "sniper_pool.summary"), "sniper_pool.summary guard scenario exists");

    for (const auto& scenario : guardScenarios) {
        expectTrue(!scenario.implemented, "guard scenario keeps implemented=false visible");
        expectTrue(etfdt::tests::shell_accounting::preservesReadOnlyBoundary(scenario), "guard scenario preserves read-only boundary");
        expectTrue(etfdt::tests::shell_accounting::mapScenarioToViewState(scenario) == "Unavailable", "implemented=false maps to Unavailable");
        expectTrue(scenario.expectedViewState != "Empty", "guard Unavailable is not Empty");
    }

    const auto stateScenarios = etfdt::tests::shell_accounting::stateContractScenarios();
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Idle"), "Idle scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Loading"), "Loading scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Loaded"), "Loaded scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Empty"), "Empty scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Unavailable"), "Unavailable scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Warning"), "Warning scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Error"), "Error scenario exists");
    expectTrue(etfdt::tests::shell_accounting::hasViewStateScenario(stateScenarios, "Stale"), "Stale scenario exists");

    for (const auto& scenario : stateScenarios) {
        expectTrue(etfdt::tests::shell_accounting::mapScenarioToViewState(scenario) == scenario.expectedViewState, "state scenario maps to expected view state");
        expectTrue(etfdt::tests::shell_accounting::preservesReadOnlyBoundary(scenario), "state scenario preserves readOnly=true/writeEnabled=false");
    }

    return gFailures == 0 ? 0 : 1;
}
