#include "FakeAccountingClientScenario.h"

#include <algorithm>

namespace etfdt::tests::shell_accounting_fake {

namespace {

FakeAccountingClientScenario guardScenario(
    std::string scenarioName,
    std::string actionName,
    std::string status)
{
    FakeAccountingClientPayload payload;
    payload.actionName = std::move(actionName);
    payload.implemented = false;
    payload.readOnly = true;
    payload.writeEnabled = false;
    payload.payloadStatus = std::move(status);
    payload.dataQualityStatus = "UNAVAILABLE";
    payload.hasRows = false;
    payload.issues.push_back({"GUARD_NOT_AVAILABLE", "ERROR", "Guard payload is not implemented.", true, "fake"});

    FakeAccountingClientScenario scenario;
    scenario.scenarioName = std::move(scenarioName);
    scenario.payload = std::move(payload);
    scenario.expectedState = etfdt::shell_services::ShellAccountingViewState::Unavailable;
    scenario.expectedIssuesVisible = true;
    scenario.expectedBlockingVisible = true;
    return scenario;
}

FakeAccountingClientScenario stateScenario(
    std::string scenarioName,
    std::string status,
    bool hasRows,
    etfdt::shell_services::ShellAccountingViewState expected)
{
    FakeAccountingClientPayload payload;
    payload.actionName = "position.list";
    payload.implemented = true;
    payload.readOnly = true;
    payload.writeEnabled = false;
    payload.dataQualityStatus = std::move(status);
    payload.hasRows = hasRows;

    FakeAccountingClientScenario scenario;
    scenario.scenarioName = std::move(scenarioName);
    scenario.payload = std::move(payload);
    scenario.expectedState = expected;
    return scenario;
}

FakeAccountingClientScenario issueScenario(
    std::string scenarioName,
    etfdt::shell_services::ShellAccountingIssue issue,
    etfdt::shell_services::ShellAccountingViewState expected)
{
    FakeAccountingClientPayload payload;
    payload.actionName = "position.list";
    payload.implemented = true;
    payload.readOnly = true;
    payload.writeEnabled = false;
    payload.dataQualityStatus = expected == etfdt::shell_services::ShellAccountingViewState::Warning
        ? "WARNING"
        : "ERROR";
    payload.hasRows = expected == etfdt::shell_services::ShellAccountingViewState::Warning;
    payload.issues.push_back(std::move(issue));

    FakeAccountingClientScenario scenario;
    scenario.scenarioName = std::move(scenarioName);
    scenario.payload = std::move(payload);
    scenario.expectedState = expected;
    scenario.expectedIssuesVisible = true;
    scenario.expectedBlockingVisible = scenario.payload.issues.front().blocking;
    return scenario;
}

}  // namespace

std::vector<FakeAccountingClientScenario> guardPayloadScenarios()
{
    return {
        guardScenario("position_list_guard", "position.list", "POSITION_LIST_NOT_AVAILABLE"),
        guardScenario("cash_summary_guard", "cash.summary", "CASH_SUMMARY_NOT_AVAILABLE"),
        guardScenario("portfolio_pnl_summary_guard", "portfolio.pnl.summary", "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"),
        guardScenario("base_position_summary_guard", "base_position.summary", "BASE_POSITION_SUMMARY_NOT_AVAILABLE"),
        guardScenario("sniper_pool_summary_guard", "sniper_pool.summary", "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"),
    };
}

std::vector<FakeAccountingClientScenario> stateTransitionScenarios()
{
    auto stale = stateScenario(
        "loading_to_stale",
        "OK",
        true,
        etfdt::shell_services::ShellAccountingViewState::Stale);
    stale.payload.stale = true;

    auto unavailable = stateScenario(
        "implemented_false_to_unavailable",
        "OK",
        true,
        etfdt::shell_services::ShellAccountingViewState::Unavailable);
    unavailable.payload.implemented = false;

    return {
        std::move(unavailable),
        stateScenario("loading_to_loaded", "OK", true, etfdt::shell_services::ShellAccountingViewState::Loaded),
        stateScenario("loading_to_empty", "OK", false, etfdt::shell_services::ShellAccountingViewState::Empty),
        stateScenario("loading_to_warning", "WARNING", true, etfdt::shell_services::ShellAccountingViewState::Warning),
        stateScenario("loading_to_degraded", "DEGRADED", true, etfdt::shell_services::ShellAccountingViewState::Warning),
        stateScenario("loading_to_error", "ERROR", false, etfdt::shell_services::ShellAccountingViewState::Error),
        std::move(stale),
    };
}

std::vector<FakeAccountingClientScenario> issueMappingScenarios()
{
    return {
        issueScenario(
            "market_price_missing_warning",
            {"MARKET_PRICE_MISSING", "WARNING", "Market price missing.", false, "market"},
            etfdt::shell_services::ShellAccountingViewState::Warning),
        issueScenario(
            "missing_fee_warning",
            {"MISSING_FEE", "WARNING", "Fee missing.", false, "trade"},
            etfdt::shell_services::ShellAccountingViewState::Warning),
        issueScenario(
            "negative_cash_error",
            {"NEGATIVE_CASH", "ERROR", "Negative cash.", true, "replay"},
            etfdt::shell_services::ShellAccountingViewState::Error),
        issueScenario(
            "sell_exceeds_position_error",
            {"SELL_EXCEEDS_POSITION", "ERROR", "Sell exceeds position.", true, "replay"},
            etfdt::shell_services::ShellAccountingViewState::Error),
        issueScenario(
            "multi_currency_unsupported_error",
            {"MULTI_CURRENCY_UNSUPPORTED", "ERROR", "Multi-currency unsupported.", true, "fx"},
            etfdt::shell_services::ShellAccountingViewState::Error),
        issueScenario(
            "fx_rate_missing_error",
            {"FX_RATE_MISSING", "ERROR", "FX rate missing.", true, "fx"},
            etfdt::shell_services::ShellAccountingViewState::Error),
        issueScenario(
            "replay_not_available_error",
            {"REPLAY_NOT_AVAILABLE", "ERROR", "Replay not available.", true, "replay"},
            etfdt::shell_services::ShellAccountingViewState::Error),
        guardScenario("cash_summary_not_available_issue", "cash.summary", "CASH_SUMMARY_NOT_AVAILABLE"),
        guardScenario("portfolio_pnl_summary_not_available_issue", "portfolio.pnl.summary", "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"),
        guardScenario("base_position_summary_not_available_issue", "base_position.summary", "BASE_POSITION_SUMMARY_NOT_AVAILABLE"),
        guardScenario("sniper_pool_summary_not_available_issue", "sniper_pool.summary", "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"),
    };
}

FakeAccountingClientScenario privacyScenario()
{
    auto scenario = stateScenario(
        "privacy_display",
        "OK",
        true,
        etfdt::shell_services::ShellAccountingViewState::Loaded);
    scenario.payload.amountRawText = "1000.00 CNY";
    scenario.payload.amountMaskedText = "***";
    scenario.payload.issues.push_back({"MARKET_PRICE_MISSING", "WARNING", "Market price missing.", false, "market"});
    scenario.expectedIssuesVisible = true;
    scenario.expectedPrivacyDisplayText = "***";
    return scenario;
}

FakeAccountingClientScenario noTradeScenario()
{
    auto scenario = stateScenario(
        "no_trade_action",
        "OK",
        true,
        etfdt::shell_services::ShellAccountingViewState::Loaded);
    scenario.payload.displayRows = {
        "basePosition.sellableAboveBaseAmountText",
        "sniperPool.remainingAmountText",
        "sniperPool.completed",
    };
    return scenario;
}

bool hasActionScenario(
    const std::vector<FakeAccountingClientScenario>& scenarios,
    const std::string& actionName)
{
    return std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
        return scenario.payload.actionName == actionName;
    });
}

bool hasScenario(
    const std::vector<FakeAccountingClientScenario>& scenarios,
    const std::string& scenarioName)
{
    return std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
        return scenario.scenarioName == scenarioName;
    });
}

}  // namespace etfdt::tests::shell_accounting_fake
