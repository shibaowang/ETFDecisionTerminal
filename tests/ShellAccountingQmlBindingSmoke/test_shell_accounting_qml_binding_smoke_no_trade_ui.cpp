#include "ShellAccountingQmlSmokeExpectedBinding.h"
#include "ShellAccountingQmlSmokeScenario.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_binding_smoke;

int main()
{
    for (const auto& token : forbiddenTradeUiTokens()) {
        if (token.empty()) {
            std::cerr << "Forbidden no-trade UI token descriptor must not be empty.\n";
            return 1;
        }
    }

    for (const auto& scenario : stateMatrixScenarios()) {
        if (scenario.expectedAllowsTradeButton || scenario.expectedAllowsTradeDraft ||
            scenario.expectedAllowsStrategyAction || scenario.expectedAllowsBrokerOrder) {
            std::cerr << "State scenario allows forbidden trade UI: " << scenario.scenarioName << '\n';
            return 1;
        }
    }

    for (const auto& scenario : guardPayloadScenarios()) {
        if (scenario.expectedAllowsTradeButton || scenario.expectedAllowsTradeDraft ||
            scenario.expectedAllowsStrategyAction || scenario.expectedAllowsBrokerOrder) {
            std::cerr << "Guard scenario allows forbidden trade UI: " << scenario.scenarioName << '\n';
            return 1;
        }
    }

    const auto descriptors = forbiddenTradeUiTokens();
    bool hasSellableBoundary = false;
    bool hasRemainingBoundary = false;
    bool hasCompletedBoundary = false;
    for (const auto& descriptor : descriptors) {
        hasSellableBoundary =
            hasSellableBoundary || descriptor == "sellableAboveBaseAmountText is not a sell suggestion";
        hasRemainingBoundary =
            hasRemainingBoundary || descriptor == "remainingAmountText is not a buy suggestion";
        hasCompletedBoundary =
            hasCompletedBoundary || descriptor == "completed is not calculated by QML";
    }
    if (!hasSellableBoundary || !hasRemainingBoundary || !hasCompletedBoundary) {
        std::cerr << "No-trade descriptors must lock sellable, remaining, and completed display boundaries.\n";
        return 1;
    }
    return 0;
}
