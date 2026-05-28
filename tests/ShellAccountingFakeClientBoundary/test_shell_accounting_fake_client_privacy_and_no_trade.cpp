#include "FakeAccountingClientScenario.h"

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
    const auto privacy = etfdt::tests::shell_accounting_fake::privacyScenario();
    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    controller.applyStateSnapshot(etfdt::tests::shell_accounting_fake::toStateSnapshot(privacy.payload));
    const auto displayText = etfdt::tests::shell_accounting_fake::toDisplayText(privacy.payload);
    controller.setPrivacyMode(false);
    expectTrue(controller.displayText(displayText) == privacy.payload.amountRawText, "privacyMode=false displays rawText");
    controller.setPrivacyMode(true);
    expectTrue(controller.displayText(displayText) == privacy.expectedPrivacyDisplayText, "privacyMode=true displays maskedText");
    expectTrue(displayText.rawText == "1000.00 CNY", "privacyMode does not mutate rawText");
    expectTrue(!controller.issues().empty(), "warning issue remains visible in privacyMode");

    etfdt::shell_services::ShellAccountingStateSnapshot unavailable;
    unavailable.actionName = "position.list";
    unavailable.implemented = false;
    unavailable.dataQualityStatus = "OK";
    unavailable.hasRows = true;
    controller.applyStateSnapshot(std::move(unavailable));
    expectTrue(controller.displayText(displayText).empty(), "Unavailable does not fabricate amount");

    auto empty = privacy.payload;
    empty.dataQualityStatus = "OK";
    empty.hasRows = false;
    empty.implemented = true;
    empty.payloadStatus.clear();
    controller.applyStateSnapshot(etfdt::tests::shell_accounting_fake::toStateSnapshot(empty));
    expectTrue(controller.displayText(displayText).empty(), "Empty does not fabricate amount");

    const auto noTrade = etfdt::tests::shell_accounting_fake::noTradeScenario();
    controller.applyStateSnapshot(etfdt::tests::shell_accounting_fake::toStateSnapshot(noTrade.payload));
    expectTrue(!controller.tradeDraftGenerationEnabled(), "tradeDraftGenerationEnabled=false");
    expectTrue(!controller.tradeSuggestionEnabled(), "tradeSuggestionEnabled=false");
    expectTrue(!controller.strategyExecutionEnabled(), "strategyExecutionEnabled=false");
    expectTrue(!controller.brokerOrderEnabled(), "brokerOrderEnabled=false");
    expectTrue(noTrade.payload.displayRows.size() == 3, "basePosition/sniperPool display rows are descriptors only");

    return gFailures == 0 ? 0 : 1;
}
