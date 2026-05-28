#include "PositionListFirstStageExpectedContract.h"
#include "PositionListFirstStageScenario.h"

#include "AccountingNoWriteTestUtils.h"

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
    const auto scenarios = etfdt::tests::position_list_first_stage::firstStageScenarios();
    expectTrue(!scenarios.empty(), "first-stage scenarios exist");

    for (const auto& scenario : scenarios) {
        expectTrue(!scenario.currentGuardImplemented, scenario.scenarioName + " keeps current guard implemented=false");
        expectTrue(scenario.expectedReadOnly, scenario.scenarioName + " expects readOnly=true");
        expectTrue(!scenario.expectedWriteEnabled, scenario.scenarioName + " expects writeEnabled=false");
        expectTrue(scenario.expectedNoWrite, scenario.scenarioName + " requires no-write");
        expectTrue(!scenario.qmlAllowed, scenario.scenarioName + " excludes QML");
    }

    expectTrue(
        etfdt::tests::position_list_first_stage::hasScenario(scenarios, "empty_ledger_ok"),
        "empty ledger scenario exists");
    expectTrue(
        etfdt::tests::position_list_first_stage::hasScenario(scenarios, "single_buy_ok"),
        "single BUY scenario exists");
    expectTrue(
        etfdt::tests::position_list_first_stage::hasScenario(scenarios, "missing_market_price_warning"),
        "missing market price scenario exists");
    expectTrue(
        etfdt::tests::position_list_first_stage::hasScenario(scenarios, "multi_currency_unsupported_error"),
        "multi-currency unsupported scenario exists");
    expectTrue(
        etfdt::tests::position_list_first_stage::hasScenario(scenarios, "unsupported_scenario_error"),
        "unsupported scenario guard exists");

    const auto tables = etfdt::tests::accounting_no_write::protectedAccountingTables();
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "trade_log"), "protected tables include trade_log");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "trade_execution_group"), "protected tables include trade_execution_group");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "trade_draft"), "protected tables include trade_draft");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "cash_snapshot"), "protected tables include cash_snapshot");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "position_snapshot"), "protected tables include position_snapshot");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "portfolio_summary"), "protected tables include portfolio_summary");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(tables, "audit_log"), "protected tables include audit_log");

    const auto excluded = etfdt::tests::position_list_first_stage::explicitlyExcludedFirstStageScope();
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "trade_draft"), "future contract excludes TradeDraft");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "trading_suggestion"), "future contract excludes trade suggestions");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "qml_integration"), "future contract excludes QML");

    return gFailures == 0 ? 0 : 1;
}
