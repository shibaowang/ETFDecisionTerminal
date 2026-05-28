#include "ShellAccountingControllerConcretePortIntegrationTestUtils.h"

#include <QCoreApplication>

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

void expectCommonGuardBoundary(
    const etfdt::shell_services::tests::ControllerConcretePortGuardResult& result,
    const std::string& action,
    const std::string& issueCode)
{
    expectTrue(result.connected, "real DataServiceClient connects to local guard endpoint");
    expectTrue(result.refreshed, "controller refresh reaches concrete port");
    expectTrue(result.observedAction == action, "expected guard wrapper action called");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "guard payload maps to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::controllerIssuesContainCode(result.issues, issueCode),
        "guard issue remains visible");
    expectTrue(result.controllerReadOnly, "controller readOnly=true");
    expectTrue(!result.controllerWriteEnabled, "controller writeEnabled=false");
    expectTrue(!result.tradeDraftEnabled, "no TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "no trade suggestion");
    expectTrue(!result.strategyExecutionEnabled, "no strategy execution");
    expectTrue(!result.brokerOrderEnabled, "no broker order");
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    etfdt::shell_services::tests::FakeAccountingWrapperServer server;
    expectTrue(server.listen(), "fake guard endpoint listens");
    const auto result =
        etfdt::shell_services::tests::refreshControllerThroughConcretePort(server, "position.list");

    expectCommonGuardBoundary(result, "position.list", "POSITION_LIST_NOT_AVAILABLE");
    return gFailures == 0 ? 0 : 1;
}
