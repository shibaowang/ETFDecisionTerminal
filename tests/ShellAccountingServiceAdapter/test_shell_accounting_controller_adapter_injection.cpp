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

    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    expectTrue(!controller.hasServiceAdapter(), "controller defaults to no service adapter");
    expectTrue(controller.readOnly(), "controller readOnly=true before adapter injection");
    expectTrue(!controller.writeEnabled(), "controller writeEnabled=false before adapter injection");

    auto fakeAdapter = std::make_shared<FakeShellAccountingServiceAdapter>(
        makeGuardUnavailableResult("position.list", "POSITION_LIST_NOT_AVAILABLE"));

    controller.setServiceAdapter(fakeAdapter);
    expectTrue(controller.hasServiceAdapter(), "controller reports adapter after injection");
    expectTrue(fakeAdapter->callCount() == 0, "setting adapter does not trigger fetch call");
    expectTrue(controller.readOnly(), "controller readOnly=true after adapter injection");
    expectTrue(!controller.writeEnabled(), "controller writeEnabled=false after adapter injection");
    expectTrue(!controller.tradeDraftGenerationEnabled(), "adapter injection does not enable TradeDraft");
    expectTrue(!controller.tradeSuggestionEnabled(), "adapter injection does not enable trade suggestion");
    expectTrue(!controller.strategyExecutionEnabled(), "adapter injection does not enable strategy");
    expectTrue(!controller.brokerOrderEnabled(), "adapter injection does not enable broker order");

    controller.clearServiceAdapter();
    expectTrue(!controller.hasServiceAdapter(), "controller clears adapter");
    expectTrue(fakeAdapter->callCount() == 0, "clearing adapter does not trigger fetch call");

    controller.setServiceAdapter(fakeAdapter);
    controller.reset();
    expectTrue(!controller.hasServiceAdapter(), "reset clears adapter");
    expectTrue(fakeAdapter->callCount() == 0, "reset does not trigger fetch call");

    return gFailures == 0 ? 0 : 1;
}
