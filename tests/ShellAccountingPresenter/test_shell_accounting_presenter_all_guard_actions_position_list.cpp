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
        etfdt::shell_services::tests::refreshPresenterActionThroughConcretePort(
            server,
            "position.list");

    expectTrue(result.connected, "DataServiceClient connects to local guard endpoint");
    expectTrue(result.refreshed, "presenter refreshPositionList reaches concrete port");
    expectTrue(result.observedAction == "position.list", "position.list wrapper is called");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "position.list guard maps to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(
            result.issues,
            "POSITION_LIST_NOT_AVAILABLE"),
        "POSITION_LIST_NOT_AVAILABLE remains visible");
    expectTrue(result.positionRowCount == 0, "position rows are not fabricated");
    expectTrue(result.state != etfdt::shell_services::ShellAccountingViewState::Empty, "Unavailable is not Empty");
    expectTrue(result.presenterReadOnly, "presenter readOnly=true");
    expectTrue(!result.presenterWriteEnabled, "presenter writeEnabled=false");
    expectTrue(!result.tradeDraftEnabled, "no TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "no trade suggestion");

    return gFailures == 0 ? 0 : 1;
}
