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

    etfdt::shell_services::ShellAccountingPresenter presenter;
    presenter.refreshPositionList();
    expectTrue(
        presenter.statusObject().state() == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "refresh without controller maps to Unavailable");
    expectTrue(
        presenter.issueListModel().issues().front().code == "CONTROLLER_NOT_CONFIGURED",
        "refresh without controller exposes controller boundary issue");

    presenter.setPrivacyMode(true);
    etfdt::shell_services::tests::FakeAccountingWrapperServer server;
    expectTrue(server.listen(), "fake guard endpoint listens");
    const auto result =
        etfdt::shell_services::tests::refreshPresenterPositionListThroughConcretePort(
            server,
            true,
            true);

    expectTrue(result.refreshed, "refreshAllReadOnly reaches concrete port");
    expectTrue(
        result.observedActions == std::vector<std::string>{
            "position.list",
            "cash.summary",
            "portfolio.pnl.summary",
            "base_position.summary",
            "sniper_pool.summary",
        },
        "refreshAllReadOnly delegates to all read-only guard actions in fixed order");
    expectTrue(
        result.state == etfdt::shell_services::ShellAccountingViewState::Unavailable,
        "refreshAllReadOnly maps guard payload to Unavailable");
    expectTrue(
        etfdt::shell_services::tests::presenterIssuesContainCode(result.issues, "POSITION_LIST_NOT_AVAILABLE"),
        "refreshAllReadOnly keeps guard issue visible");
    expectTrue(result.presenterReadOnly, "presenter remains read-only");
    expectTrue(!result.presenterWriteEnabled, "presenter remains write disabled");
    expectTrue(!result.tradeDraftEnabled, "refresh does not enable TradeDraft");
    expectTrue(!result.tradeSuggestionEnabled, "refresh does not enable trade suggestion");
    expectTrue(!result.strategyExecutionEnabled, "refresh does not enable strategy");
    expectTrue(!result.brokerSubmissionEnabled, "refresh does not enable broker order");

    return gFailures == 0 ? 0 : 1;
}
