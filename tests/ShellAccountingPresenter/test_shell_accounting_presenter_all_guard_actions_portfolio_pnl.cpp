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
            "portfolio.pnl.summary");

    expectTrue(result.refreshed, "presenter refreshPortfolioPnlSummary reaches concrete port");
    expectTrue(result.observedAction == "portfolio.pnl.summary", "portfolio.pnl.summary wrapper is called");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "portfolio.pnl.summary guard maps to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(
            result.issues,
            "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"),
        "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE remains visible");
    expectTrue(result.positionRowCount == 0, "pnl refresh does not fabricate rows");
    expectTrue(result.presenterReadOnly, "presenter readOnly=true");
    expectTrue(!result.presenterWriteEnabled, "presenter writeEnabled=false");
    expectTrue(!result.tradeDraftEnabled, "no TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "no trade suggestion");

    return gFailures == 0 ? 0 : 1;
}
