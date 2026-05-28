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
            "sniper_pool.summary");

    expectTrue(result.refreshed, "presenter refreshSniperPoolSummary reaches concrete port");
    expectTrue(result.observedAction == "sniper_pool.summary", "sniper_pool.summary wrapper is called");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "sniper_pool.summary guard maps to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(
            result.issues,
            "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"),
        "SNIPER_POOL_SUMMARY_NOT_AVAILABLE remains visible");
    expectTrue(!result.tradeDraftEnabled, "sniper pool refresh does not create TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "sniper pool refresh does not create trade suggestion");
    expectTrue(!result.strategyExecutionEnabled, "sniper pool refresh does not execute strategy");
    expectTrue(!result.brokerSubmissionEnabled, "sniper pool refresh does not submit broker order");

    return gFailures == 0 ? 0 : 1;
}
