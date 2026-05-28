#include "ShellAccountingStateMappingTestUtils.h"

#include <array>

namespace etfdt::tests::shell_accounting {

namespace {

bool isGuardUnavailableStatus(const std::string& status)
{
    constexpr std::array<const char*, 5> kStatuses{
        "POSITION_LIST_NOT_AVAILABLE",
        "CASH_SUMMARY_NOT_AVAILABLE",
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE",
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE",
    };

    for (const auto* unavailableStatus : kStatuses) {
        if (status == unavailableStatus) {
            return true;
        }
    }

    return false;
}

}  // namespace

std::string mapScenarioToViewState(const ShellAccountingControllerScenario& scenario)
{
    if (scenario.dataQualityStatus == "LOADING") {
        return "Loading";
    }

    if (!scenario.implemented || isGuardUnavailableStatus(scenario.payloadStatus)) {
        return "Unavailable";
    }

    if (scenario.dataQualityStatus == "IDLE") {
        return "Idle";
    }

    if (scenario.dataQualityStatus == "OK") {
        return scenario.hasRows ? "Loaded" : "Empty";
    }

    if (scenario.dataQualityStatus == "WARNING" || scenario.dataQualityStatus == "DEGRADED") {
        return "Warning";
    }

    if (scenario.dataQualityStatus == "ERROR") {
        return "Error";
    }

    if (scenario.dataQualityStatus == "STALE") {
        return "Stale";
    }

    if (scenario.dataQualityStatus == "UNAVAILABLE") {
        return "Unavailable";
    }

    return "Error";
}

std::string mapIssueToViewState(const ShellAccountingIssueDescriptor& issue)
{
    if (issue.code == "MARKET_PRICE_MISSING" || issue.code == "MISSING_FEE") {
        return "Warning";
    }

    if (issue.code == "REPLAY_NOT_AVAILABLE") {
        return issue.blocking ? "Error" : "Unavailable";
    }

    if (issue.code == "NEGATIVE_CASH" || issue.code == "SELL_EXCEEDS_POSITION" ||
        issue.code == "MULTI_CURRENCY_UNSUPPORTED" || issue.code == "FX_RATE_MISSING") {
        return "Error";
    }

    return issue.blocking ? "Error" : "Warning";
}

std::string displayTextForPrivacy(
    const std::string& rawText,
    const std::string& maskedText,
    bool privacyMode,
    const std::string& viewState)
{
    if (viewState == "Unavailable" || viewState == "Empty") {
        return {};
    }

    return privacyMode ? maskedText : rawText;
}

bool preservesReadOnlyBoundary(const ShellAccountingControllerScenario& scenario)
{
    return scenario.readOnly && !scenario.writeEnabled;
}

bool hidesTradeActions(const ShellAccountingTradeActionBoundary& boundary)
{
    return !boundary.allowsTradeAction && !boundary.generatesTradeDraft &&
        !boundary.generatesTradeSuggestion && !boundary.executesStrategy &&
        !boundary.sendsBrokerOrder && !boundary.qmlCalculates;
}

}  // namespace etfdt::tests::shell_accounting
