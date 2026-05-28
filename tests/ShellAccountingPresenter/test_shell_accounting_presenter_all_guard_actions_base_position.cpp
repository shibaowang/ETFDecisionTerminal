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
            "base_position.summary");

    expectTrue(result.refreshed, "presenter refreshBasePositionSummary reaches concrete port");
    expectTrue(result.observedAction == "base_position.summary", "base_position.summary wrapper is called");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "base_position.summary guard maps to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(
            result.issues,
            "BASE_POSITION_SUMMARY_NOT_AVAILABLE"),
        "BASE_POSITION_SUMMARY_NOT_AVAILABLE remains visible");
    expectTrue(!result.tradeDraftEnabled, "base position refresh does not create TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "base position refresh does not create trade suggestion");
    expectTrue(!result.strategyExecutionEnabled, "base position refresh does not execute strategy");
    expectTrue(!result.brokerSubmissionEnabled, "base position refresh does not submit broker order");

    return gFailures == 0 ? 0 : 1;
}
