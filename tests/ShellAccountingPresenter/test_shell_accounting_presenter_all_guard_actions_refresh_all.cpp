#include "ShellAccountingPresenterConcretePortTestUtils.h"

#include <QCoreApplication>

#include <iostream>
#include <string_view>
#include <vector>

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
        etfdt::shell_services::tests::refreshPresenterAllReadOnlyThroughConcretePort(server);

    const std::vector<std::string> expectedActions{
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
    };

    expectTrue(result.refreshed, "refreshAllReadOnly reaches all guard wrappers");
    expectTrue(result.observedActions == expectedActions, "refreshAllReadOnly uses the fixed read-only action order");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "refreshAllReadOnly aggregate guard state is Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "POSITION_LIST_NOT_AVAILABLE"),
        "position issue is visible");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "CASH_SUMMARY_NOT_AVAILABLE"),
        "cash issue is visible");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"),
        "portfolio pnl issue is visible");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "BASE_POSITION_SUMMARY_NOT_AVAILABLE"),
        "base position issue is visible");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"),
        "sniper pool issue is visible");
    expectTrue(result.presenterReadOnly, "presenter readOnly=true");
    expectTrue(!result.presenterWriteEnabled, "presenter writeEnabled=false");
    expectTrue(!result.tradeDraftEnabled, "no TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "no trade suggestion");

    return gFailures == 0 ? 0 : 1;
}
