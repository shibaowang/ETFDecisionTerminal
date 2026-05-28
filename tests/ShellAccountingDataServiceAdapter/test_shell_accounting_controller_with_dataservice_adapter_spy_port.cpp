#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "SpyShellAccountingDataServiceClientPort.h"

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
    using etfdt::shell_services::ShellAccountingDataServiceAdapter;
    using etfdt::shell_services::ShellAccountingReadOnlyController;
    using etfdt::shell_services::ShellAccountingViewState;
    using etfdt::shell_services::tests::SpyShellAccountingDataServiceClientPort;
    using etfdt::shell_services::tests::makeLiveCallSkeletonRequest;
    using etfdt::shell_services::tests::makeOkClientResponse;
    using etfdt::shell_services::tests::makePortIssue;

    auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
    auto response = makeOkClientResponse("position.list");
    response.warnings.push_back(makePortIssue("MARKET_PRICE_MISSING", "WARNING", false));
    spy->setNextResponse(response);

    auto adapter = std::make_shared<ShellAccountingDataServiceAdapter>(spy);
    ShellAccountingReadOnlyController controller;
    controller.setServiceAdapter(adapter);

    controller.refreshPositionList(makeLiveCallSkeletonRequest("position.list"));

    expectTrue(spy->callCount("callPositionList") == 1, "controller refresh reaches spy port through adapter");
    expectTrue(controller.currentActionName() == "position.list", "controller action is mapped");
    expectTrue(controller.currentState() == ShellAccountingViewState::Loaded, "controller state maps from port response");
    expectTrue(!controller.issues().empty(), "controller exposes mapped issue");
    expectTrue(controller.issues().front().code == "MARKET_PRICE_MISSING", "controller retains issue code");
    expectTrue(controller.readOnly(), "controller remains read-only");
    expectTrue(!controller.writeEnabled(), "controller remains write disabled");
    expectTrue(!controller.tradeDraftGenerationEnabled(), "controller does not enable TradeDraft");
    expectTrue(!controller.tradeSuggestionEnabled(), "controller does not enable trade suggestion");

    return gFailures == 0 ? 0 : 1;
}
