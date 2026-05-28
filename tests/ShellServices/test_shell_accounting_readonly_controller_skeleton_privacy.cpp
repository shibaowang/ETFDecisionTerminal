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

etfdt::shell_services::ShellAccountingStateSnapshot snapshot(
    std::string status,
    bool hasRows)
{
    etfdt::shell_services::ShellAccountingStateSnapshot value;
    value.actionName = "position.list";
    value.implemented = true;
    value.dataQualityStatus = std::move(status);
    value.hasRows = hasRows;
    return value;
}

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    etfdt::shell_services::ShellAccountingDisplayText text{"1000.00 CNY", "***"};

    controller.applyStateSnapshot(snapshot("OK", true));
    controller.setPrivacyMode(false);
    expectTrue(controller.displayText(text) == "1000.00 CNY", "privacyMode=false shows rawText");
    controller.setPrivacyMode(true);
    expectTrue(controller.displayText(text) == "***", "privacyMode=true shows maskedText");
    expectTrue(text.rawText == "1000.00 CNY", "masking does not mutate rawText");

    controller.applyStateSnapshot(snapshot("OK", false));
    expectTrue(controller.displayText(text).empty(), "Empty state does not fabricate amount");

    etfdt::shell_services::ShellAccountingStateSnapshot unavailable;
    unavailable.actionName = "position.list";
    unavailable.implemented = false;
    unavailable.dataQualityStatus = "OK";
    unavailable.hasRows = true;
    unavailable.issues.push_back({"MARKET_PRICE_MISSING", "WARNING", "Market price missing.", false, "market_price"});
    controller.applyStateSnapshot(std::move(unavailable));
    expectTrue(controller.displayText(text).empty(), "Unavailable state does not fabricate amount");
    expectTrue(!controller.issues().empty(), "issues remain visible in privacy mode");

    return gFailures == 0 ? 0 : 1;
}
