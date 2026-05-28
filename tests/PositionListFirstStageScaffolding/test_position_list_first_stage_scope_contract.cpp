#include "PositionListFirstStageExpectedContract.h"
#include "PositionListFirstStageScenario.h"

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
    const auto allowed = etfdt::tests::position_list_first_stage::allowedFirstStageScope();
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "request_validation"), "request validation allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "dataaccess_trade_facts_readonly_query"), "DataAccess trade facts reader allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "trade_fact_row_to_trade_fact_dto_mapping"), "TradeFactRow to TradeFactDto mapping allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "replay_request_dto_mapping"), "ReplayRequestDto mapping allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "accounting_engine_replay_readonly"), "AccountingEngine replayReadOnly allowed in future implementation");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "position_list_response_mapping"), "PositionListResponse mapping allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "empty_ledger"), "empty ledger allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "single_buy"), "single BUY allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "missing_market_price_warning"), "missing market price warning allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "multi_currency_unsupported_error"), "multi-currency unsupported error allowed");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "no_write_integration_tests"), "no-write integration tests required");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(allowed, "guard_fallback"), "guard fallback required");

    const auto excluded = etfdt::tests::position_list_first_stage::explicitlyExcludedFirstStageScope();
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "cash_summary_implementation"), "cash.summary implementation excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "portfolio_pnl_summary_implementation"), "portfolio.pnl.summary implementation excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "base_position_summary_implementation"), "base_position.summary implementation excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "sniper_pool_summary_implementation"), "sniper_pool.summary implementation excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "snapshot_write"), "snapshot write excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "trade_log_write"), "TradeLog write excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "trade_draft"), "TradeDraft excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "trading_suggestion"), "trading suggestion excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "strategy_execution"), "strategy execution excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "qml_integration"), "QML integration excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "market_data_service"), "market data service excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "fx_service"), "FX service excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "multi_currency_valuation"), "multi-currency valuation excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "base_position_calculation"), "basePosition calculation excluded");
    expectTrue(etfdt::tests::position_list_first_stage::containsValue(excluded, "sniper_pool_calculation"), "sniperPool calculation excluded");

    return gFailures == 0 ? 0 : 1;
}
