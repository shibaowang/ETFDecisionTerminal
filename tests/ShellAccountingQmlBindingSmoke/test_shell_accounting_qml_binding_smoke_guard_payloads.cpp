#include "ShellAccountingQmlSmokeScenario.h"

#include <iostream>
#include <set>
#include <string>

using namespace etfdt::tests::shell_accounting_qml_binding_smoke;

int main()
{
    const auto scenarios = guardPayloadScenarios();
    const std::set<std::string> expectedStatuses{
        "POSITION_LIST_NOT_AVAILABLE",
        "CASH_SUMMARY_NOT_AVAILABLE",
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE",
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE",
    };

    std::set<std::string> actualStatuses;
    for (const auto& scenario : scenarios) {
        actualStatuses.insert(scenario.payloadStatus);
        if (scenario.expectedVisibleState != "Unavailable") {
            std::cerr << "Guard payload must map to Unavailable: " << scenario.payloadStatus << '\n';
            return 1;
        }
        if (scenario.expectedVisibleState == "Empty" || scenario.hasRows) {
            std::cerr << "Guard payload must not display as Empty or fabricate rows: " << scenario.payloadStatus << '\n';
            return 1;
        }
        if (scenario.issues.empty() || !scenario.expectedIssueVisible || !scenario.expectedBlockingVisible) {
            std::cerr << "Guard issue must remain visible and blocking: " << scenario.payloadStatus << '\n';
            return 1;
        }
        if (scenario.expectedAllowsTradeButton || scenario.expectedAllowsTradeDraft ||
            scenario.expectedAllowsStrategyAction || scenario.expectedAllowsBrokerOrder) {
            std::cerr << "Guard payload must not enable trade UI: " << scenario.payloadStatus << '\n';
            return 1;
        }
    }

    if (actualStatuses != expectedStatuses) {
        std::cerr << "Guard payload smoke matrix does not cover all five guard statuses.\n";
        return 1;
    }
    return 0;
}
