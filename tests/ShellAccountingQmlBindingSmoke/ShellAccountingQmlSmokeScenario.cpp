#include "ShellAccountingQmlSmokeScenario.h"

namespace etfdt::tests::shell_accounting_qml_binding_smoke {
namespace {

SmokeIssue issue(
    std::string code,
    std::string level,
    std::string message,
    bool blocking)
{
    return {std::move(code), std::move(level), std::move(message), blocking};
}

ShellAccountingQmlSmokeScenario scenario(
    std::string name,
    std::string state,
    std::string visibleState,
    bool implemented,
    bool hasRows)
{
    return {
        std::move(name),
        "ShellAccountingStatusObject",
        std::move(state),
        "OK",
        "AVAILABLE",
        implemented,
        true,
        false,
        hasRows,
        {},
        false,
        std::move(visibleState),
        false,
        false,
        "displayText-only",
        false,
        false,
        false,
        false,
        "read-only state display",
    };
}

ShellAccountingQmlSmokeScenario guardScenario(
    std::string name,
    std::string objectName,
    std::string payloadStatus,
    std::string issueCode)
{
    return {
        std::move(name),
        std::move(objectName),
        "Unavailable",
        std::move(payloadStatus),
        "UNAVAILABLE",
        false,
        true,
        false,
        false,
        {issue(issueCode, "ERROR", "Guard payload is not available", true)},
        false,
        "Unavailable",
        true,
        true,
        "unavailable-does-not-fake-data",
        false,
        false,
        false,
        false,
        "guard payload must stay visible as unavailable",
    };
}

}  // namespace

std::vector<ShellAccountingQmlSmokeScenario> stateMatrixScenarios()
{
    auto scenarios = std::vector<ShellAccountingQmlSmokeScenario>{
        scenario("idle", "Idle", "Idle", true, false),
        scenario("loading", "Loading", "Loading", true, false),
        scenario("loaded", "Loaded", "Loaded", true, true),
        scenario("empty", "Empty", "Empty", true, false),
        scenario("unavailable", "Unavailable", "Unavailable", false, false),
        scenario("warning", "Warning", "Warning", true, true),
        scenario("error", "Error", "Error", true, false),
        scenario("stale", "Stale", "Stale", true, true),
    };
    scenarios[5].issues.push_back(issue("MARKET_PRICE_MISSING", "WARNING", "Market price missing", false));
    scenarios[6].issues.push_back(issue("MULTI_CURRENCY_UNSUPPORTED", "ERROR", "Unsupported currency", true));
    scenarios[7].dataQualityStatus = "STALE";
    return scenarios;
}

std::vector<ShellAccountingQmlSmokeScenario> guardPayloadScenarios()
{
    return {
        guardScenario(
            "position list not available",
            "ShellPositionListModel",
            "POSITION_LIST_NOT_AVAILABLE",
            "POSITION_LIST_NOT_AVAILABLE"),
        guardScenario(
            "cash summary not available",
            "ShellCashSummaryObject",
            "CASH_SUMMARY_NOT_AVAILABLE",
            "CASH_SUMMARY_NOT_AVAILABLE"),
        guardScenario(
            "portfolio pnl summary not available",
            "ShellPortfolioPnlObject",
            "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
            "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"),
        guardScenario(
            "base position summary not available",
            "ShellBasePositionObject",
            "BASE_POSITION_SUMMARY_NOT_AVAILABLE",
            "BASE_POSITION_SUMMARY_NOT_AVAILABLE"),
        guardScenario(
            "sniper pool summary not available",
            "ShellSniperPoolObject",
            "SNIPER_POOL_SUMMARY_NOT_AVAILABLE",
            "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"),
    };
}

ShellAccountingQmlSmokeScenario privacyScenario(bool privacyMode)
{
    auto result = scenario("privacy display", "Loaded", "Loaded", true, true);
    result.objectName = "PrivacyAmountText";
    result.privacyMode = privacyMode;
    result.expectedDisplayTextPolicy = privacyMode ? "displayText=maskedText" : "displayText=rawText";
    result.notes = "rawText remains ShellServices-owned and unchanged";
    return result;
}

ShellAccountingQmlSmokeScenario warningIssueScenario()
{
    auto result = scenario("warning issue", "Warning", "Warning", true, true);
    result.issues.push_back(issue("MARKET_PRICE_MISSING", "WARNING", "Market price missing", false));
    result.expectedIssueVisible = true;
    result.expectedBlockingVisible = false;
    return result;
}

}  // namespace etfdt::tests::shell_accounting_qml_binding_smoke
