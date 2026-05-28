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
    const auto scenarios = etfdt::tests::shell_accounting_fake::guardPayloadScenarios();
    expectTrue(etfdt::tests::shell_accounting_fake::hasActionScenario(scenarios, "position.list"), "position.list guard payload scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasActionScenario(scenarios, "cash.summary"), "cash.summary guard payload scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasActionScenario(scenarios, "portfolio.pnl.summary"), "portfolio.pnl.summary guard payload scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasActionScenario(scenarios, "base_position.summary"), "base_position.summary guard payload scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasActionScenario(scenarios, "sniper_pool.summary"), "sniper_pool.summary guard payload scenario exists");

    for (const auto& scenario : scenarios) {
        etfdt::shell_services::ShellAccountingReadOnlyController controller;
        controller.applyStateSnapshot(etfdt::tests::shell_accounting_fake::toStateSnapshot(scenario.payload));
        expectTrue(controller.currentActionName() == scenario.payload.actionName, "actionName is preserved");
        expectTrue(!scenario.payload.implemented, "guard payload keeps implemented=false");
        expectTrue(scenario.payload.readOnly, "guard payload keeps readOnly=true");
        expectTrue(!scenario.payload.writeEnabled, "guard payload keeps writeEnabled=false");
        expectTrue(controller.readOnly(), "controller readOnly=true");
        expectTrue(!controller.writeEnabled(), "controller writeEnabled=false");
        expectTrue(controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Unavailable, "guard payload maps to Unavailable");
        expectTrue(controller.currentState() != etfdt::shell_services::ShellAccountingViewState::Empty, "Unavailable is not Empty");
        expectTrue(!controller.issues().empty(), "issue list is visible");
        expectTrue(controller.issues().front().blocking, "blocking issue is visible");
        expectTrue(!controller.tradeDraftGenerationEnabled(), "guard payload does not generate TradeDraft");
        expectTrue(!controller.tradeSuggestionEnabled(), "guard payload does not generate trade suggestion");
    }

    return gFailures == 0 ? 0 : 1;
}
