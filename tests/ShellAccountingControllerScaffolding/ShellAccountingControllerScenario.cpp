#include "ShellAccountingControllerScenario.h"

#include <algorithm>

namespace etfdt::tests::shell_accounting {

namespace {

ShellAccountingControllerScenario makeGuardScenario(
    std::string scenarioName,
    std::string actionName,
    std::string payloadStatus)
{
    return ShellAccountingControllerScenario{
        std::move(scenarioName),
        std::move(actionName),
        false,
        true,
        false,
        "UNAVAILABLE",
        std::move(payloadStatus),
        {},
        false,
        "Unavailable",
        false,
        false,
        false,
        false,
        false,
        "Current guard payload maps to ShellServices Unavailable state, not Empty.",
    };
}

}  // namespace

std::vector<ShellAccountingControllerScenario> guardPayloadScenarios()
{
    return {
        makeGuardScenario("position_list_guard_unavailable", "position.list", "POSITION_LIST_NOT_AVAILABLE"),
        makeGuardScenario("cash_summary_guard_unavailable", "cash.summary", "CASH_SUMMARY_NOT_AVAILABLE"),
        makeGuardScenario(
            "portfolio_pnl_summary_guard_unavailable",
            "portfolio.pnl.summary",
            "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"),
        makeGuardScenario(
            "base_position_summary_guard_unavailable",
            "base_position.summary",
            "BASE_POSITION_SUMMARY_NOT_AVAILABLE"),
        makeGuardScenario(
            "sniper_pool_summary_guard_unavailable",
            "sniper_pool.summary",
            "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"),
    };
}

std::vector<ShellAccountingControllerScenario> stateContractScenarios()
{
    return {
        {"idle_before_refresh", "position.list", true, true, false, "IDLE", "", {}, false, "Idle"},
        {"loading_refresh", "position.list", true, true, false, "LOADING", "", {}, false, "Loading"},
        {"loaded_with_rows", "position.list", true, true, false, "OK", "", {}, true, "Loaded"},
        {"empty_success", "position.list", true, true, false, "OK", "", {}, false, "Empty"},
        {"unavailable_source", "position.list", true, true, false, "UNAVAILABLE", "REPLAY_NOT_AVAILABLE", {}, false, "Unavailable"},
        {"warning_degraded", "position.list", true, true, false, "DEGRADED", "", {{"MARKET_PRICE_MISSING", "WARNING", "missing market price", false, "market_price"}}, true, "Warning", true},
        {"error_blocking", "position.list", true, true, false, "ERROR", "", {{"NEGATIVE_CASH", "ERROR", "negative cash", true, "replay"}}, false, "Error", true, true},
        {"stale_previous_data", "position.list", true, true, false, "STALE", "", {}, true, "Stale"},
    };
}

std::vector<ShellAccountingIssueDescriptor> issueContractScenarios()
{
    return {
        {"MARKET_PRICE_MISSING", "WARNING", "Market price is missing.", false, "market_price"},
        {"MISSING_FEE", "WARNING", "Trade fee is missing.", false, "trade_fact"},
        {"NEGATIVE_CASH", "ERROR", "Cash would become negative.", true, "replay"},
        {"SELL_EXCEEDS_POSITION", "ERROR", "Sell quantity exceeds position.", true, "replay"},
        {"MULTI_CURRENCY_UNSUPPORTED", "ERROR", "Multi-currency valuation is unsupported.", true, "fx"},
        {"FX_RATE_MISSING", "ERROR", "FX rate is missing.", true, "fx"},
        {"REPLAY_NOT_AVAILABLE", "ERROR", "Replay is not available.", true, "replay"},
    };
}

std::vector<ShellAccountingTradeActionBoundary> noTradeActionBoundaries()
{
    return {
        {"position_list_rows", "positions", false, false, false, false, false, false, "Position rows are read-only holdings, not trade actions."},
        {"base_position_sellable", "sellableAboveBaseAmountText", false, false, false, false, false, false, "sellableAboveBaseAmountText is not a sell suggestion."},
        {"sniper_pool_remaining", "remainingAmountText", false, false, false, false, false, false, "remainingAmountText is not a buy suggestion."},
        {"sniper_pool_completed", "completed", false, false, false, false, false, false, "completed is not calculated by QML."},
        {"controller_refresh", "refreshPositionList", false, false, false, false, false, false, "Refresh is read-only and does not execute strategy."},
    };
}

std::vector<std::string> protectedAccountingTables()
{
    return {
        "trade_log",
        "trade_execution_group",
        "trade_draft",
        "cash_snapshot",
        "position_snapshot",
        "portfolio_summary",
        "audit_log",
    };
}

bool hasActionScenario(
    const std::vector<ShellAccountingControllerScenario>& scenarios,
    const std::string& actionName)
{
    return std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
        return scenario.actionName == actionName;
    });
}

bool hasViewStateScenario(
    const std::vector<ShellAccountingControllerScenario>& scenarios,
    const std::string& viewState)
{
    return std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
        return scenario.expectedViewState == viewState;
    });
}

bool hasIssueCode(
    const std::vector<ShellAccountingIssueDescriptor>& issues,
    const std::string& code)
{
    return std::any_of(issues.begin(), issues.end(), [&](const auto& issue) {
        return issue.code == code;
    });
}

}  // namespace etfdt::tests::shell_accounting
