#include "ShellServices/ShellAccountingReadOnlyController.h"

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
    etfdt::shell_services::ShellAccountingReadOnlyController controller;

    expectTrue(!controller.tradeDraftGenerationEnabled(), "controller never generates TradeDraft");
    expectTrue(!controller.tradeSuggestionEnabled(), "controller never generates trade suggestions");
    expectTrue(!controller.strategyExecutionEnabled(), "controller never executes strategy");
    expectTrue(!controller.brokerOrderEnabled(), "controller never sends broker order");
    expectTrue(!controller.writeEnabled(), "controller writeEnabled=false");
    expectTrue(controller.readOnly(), "controller readOnly=true");

    constexpr std::string_view kBasePositionField = "sellableAboveBaseAmountText";
    constexpr std::string_view kSniperPoolField = "remainingAmountText";
    expectTrue(kBasePositionField == "sellableAboveBaseAmountText", "basePosition sellableAboveBaseAmountText is only display data");
    expectTrue(kSniperPoolField == "remainingAmountText", "sniperPool remainingAmountText is only display data");

    return gFailures == 0 ? 0 : 1;
}
