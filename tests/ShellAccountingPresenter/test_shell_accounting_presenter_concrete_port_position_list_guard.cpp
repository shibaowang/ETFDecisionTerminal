#include "ShellAccountingPresenterConcretePortTestUtils.h"

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

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    etfdt::shell_services::tests::FakeAccountingWrapperServer server;
    expectTrue(server.listen(), "fake guard endpoint listens");

    const auto result =
        etfdt::shell_services::tests::refreshPresenterPositionListThroughConcretePort(server);

    expectTrue(result.connected, "real DataServiceClient connects to local guard endpoint");
    expectTrue(result.presenterHasController, "presenter has controller");
    expectTrue(result.refreshed, "presenter refresh reaches concrete port");
    expectTrue(result.observedAction == "position.list", "position.list guard wrapper is called");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "guard payload maps presenter state to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "POSITION_LIST_NOT_AVAILABLE"),
        "POSITION_LIST_NOT_AVAILABLE issue remains visible");
    expectTrue(result.presenterReadOnly, "presenter readOnly=true");
    expectTrue(!result.presenterWriteEnabled, "presenter writeEnabled=false");
    expectTrue(result.positionRowCount == 0, "position model does not return real positions");
    expectTrue(result.state != etfdt::shell_services::ShellAccountingViewState::Empty, "Unavailable is not Empty");
    expectTrue(!result.tradeDraftEnabled, "no TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "no trade suggestion");
    expectTrue(!result.strategyExecutionEnabled, "no strategy execution");
    expectTrue(!result.brokerSubmissionEnabled, "no broker order");

    return gFailures == 0 ? 0 : 1;
}
