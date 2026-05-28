#include "ShellServices/ShellAccountingPresenter.h"

#include <cstdlib>
#include <iostream>

namespace {

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingPresenter;
    using etfdt::shell_services::ShellAccountingViewState;

    ShellAccountingPresenter presenter;

    expectTrue(presenter.readOnly(), "presenter is read-only");
    expectTrue(!presenter.writeEnabled(), "presenter writeEnabled is false");
    expectTrue(!presenter.tradeDraftGenerationEnabled(), "presenter does not generate TradeDraft");
    expectTrue(!presenter.tradeSuggestionEnabled(), "presenter does not generate trade suggestions");
    expectTrue(!presenter.strategyExecutionEnabled(), "presenter does not execute strategy");
    expectTrue(!presenter.brokerSubmissionEnabled(), "presenter does not submit broker orders");
    expectTrue(!presenter.hasController(), "presenter starts without controller");
    expectTrue(presenter.statusObject().state() == ShellAccountingViewState::Idle, "status object exists and starts Idle");
    expectTrue(presenter.issueListModel().rowCount() == 0, "issue model exists and starts empty");
    expectTrue(presenter.positionListModel().rowCount() == 0, "position list model exists and starts empty");

    return 0;
}
