#include "FakeShellAccountingServiceAdapter.h"

#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <iostream>
#include <memory>
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
    using namespace etfdt::tests::shell_accounting_service_adapter;

    etfdt::shell_services::ShellAccountingServiceResult warningResult;
    warningResult.protocolSuccess = true;
    warningResult.implemented = true;
    warningResult.readOnly = true;
    warningResult.writeEnabled = false;
    warningResult.dataQualityStatus = "WARNING";
    warningResult.hasRows = true;
    warningResult.warnings.push_back({
        "MARKET_PRICE_MISSING",
        "WARNING",
        "Market price is unavailable.",
        false,
        "position.list",
    });

    auto fakeAdapter = std::make_shared<FakeShellAccountingServiceAdapter>(warningResult);
    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    controller.setServiceAdapter(fakeAdapter);
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));

    const etfdt::shell_services::ShellAccountingDisplayText amount{
        "1000.00 CNY",
        "***",
    };

    controller.setPrivacyMode(false);
    expectTrue(controller.displayText(amount) == "1000.00 CNY", "privacyMode=false returns raw text");
    controller.setPrivacyMode(true);
    expectTrue(controller.displayText(amount) == "***", "privacyMode=true returns masked text");
    expectTrue(amount.rawText == "1000.00 CNY", "privacy masking does not mutate raw text");
    expectTrue(!controller.issues().empty(), "warning remains visible under privacy mode");

    expectTrue(controller.readOnly(), "refresh keeps controller readOnly=true");
    expectTrue(!controller.writeEnabled(), "refresh keeps controller writeEnabled=false");
    expectTrue(!controller.tradeDraftGenerationEnabled(), "refresh does not generate TradeDraft");
    expectTrue(!controller.tradeSuggestionEnabled(), "refresh does not generate trade suggestion");
    expectTrue(!controller.strategyExecutionEnabled(), "refresh does not execute strategy");
    expectTrue(!controller.brokerOrderEnabled(), "refresh does not submit broker order");

    return gFailures == 0 ? 0 : 1;
}
