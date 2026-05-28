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
    const auto scenarios = etfdt::tests::shell_accounting_fake::issueMappingScenarios();
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "market_price_missing_warning"), "MARKET_PRICE_MISSING scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "missing_fee_warning"), "MISSING_FEE scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "negative_cash_error"), "NEGATIVE_CASH scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "sell_exceeds_position_error"), "SELL_EXCEEDS_POSITION scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "multi_currency_unsupported_error"), "MULTI_CURRENCY_UNSUPPORTED scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "fx_rate_missing_error"), "FX_RATE_MISSING scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "replay_not_available_error"), "REPLAY_NOT_AVAILABLE scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "cash_summary_not_available_issue"), "CASH_SUMMARY_NOT_AVAILABLE scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "portfolio_pnl_summary_not_available_issue"), "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "base_position_summary_not_available_issue"), "BASE_POSITION_SUMMARY_NOT_AVAILABLE scenario exists");
    expectTrue(etfdt::tests::shell_accounting_fake::hasScenario(scenarios, "sniper_pool_summary_not_available_issue"), "SNIPER_POOL_SUMMARY_NOT_AVAILABLE scenario exists");

    for (const auto& scenario : scenarios) {
        etfdt::shell_services::ShellAccountingReadOnlyController controller;
        controller.applyStateSnapshot(etfdt::tests::shell_accounting_fake::toStateSnapshot(scenario.payload));
        expectTrue(controller.currentState() == scenario.expectedState, "issue payload maps to expected state");
        expectTrue(!controller.issues().empty(), "issue list is retained");
        expectTrue(!controller.issues().front().code.empty(), "issue code is retained");
        expectTrue(!controller.issues().front().level.empty(), "issue level is retained");
        expectTrue(!controller.issues().front().message.empty(), "issue message is retained");
        expectTrue(!controller.issues().front().sourceId.empty(), "issue sourceId is retained");
        if (scenario.expectedBlockingVisible) {
            expectTrue(controller.issues().front().blocking, "blocking issue is retained");
        }
    }

    return gFailures == 0 ? 0 : 1;
}
