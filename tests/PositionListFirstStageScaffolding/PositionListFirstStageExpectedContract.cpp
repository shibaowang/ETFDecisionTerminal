#include "PositionListFirstStageExpectedContract.h"

#include <algorithm>

namespace etfdt::tests::position_list_first_stage {

std::vector<std::string> allowedFirstStageScope()
{
    return {
        "request_validation",
        "dataaccess_trade_facts_readonly_query",
        "trade_fact_row_to_trade_fact_dto_mapping",
        "replay_request_dto_mapping",
        "accounting_engine_replay_readonly",
        "position_list_response_mapping",
        "empty_ledger",
        "single_buy",
        "missing_market_price_warning",
        "multi_currency_unsupported_error",
        "no_write_integration_tests",
        "guard_fallback",
    };
}

std::vector<std::string> explicitlyExcludedFirstStageScope()
{
    return {
        "cash_summary_implementation",
        "portfolio_pnl_summary_implementation",
        "base_position_summary_implementation",
        "sniper_pool_summary_implementation",
        "snapshot_write",
        "trade_log_write",
        "trade_draft",
        "trading_suggestion",
        "strategy_execution",
        "qml_integration",
        "market_data_service",
        "fx_service",
        "multi_currency_valuation",
        "base_position_calculation",
        "sniper_pool_calculation",
    };
}

bool containsValue(const std::vector<std::string>& values, const std::string& expected)
{
    return std::find(values.begin(), values.end(), expected) != values.end();
}

}  // namespace etfdt::tests::position_list_first_stage
