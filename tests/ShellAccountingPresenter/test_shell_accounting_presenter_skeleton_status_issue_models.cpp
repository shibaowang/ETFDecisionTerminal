#include "ShellServices/ShellAccountingPresenter.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace {

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

etfdt::shell_services::ShellAccountingStateSnapshot snapshot(
    std::string payloadStatus,
    std::string dataQualityStatus,
    bool implemented,
    bool hasRows,
    bool stale = false)
{
    etfdt::shell_services::ShellAccountingStateSnapshot value;
    value.actionName = "position.list";
    value.implemented = implemented;
    value.readOnly = true;
    value.writeEnabled = false;
    value.payloadStatus = std::move(payloadStatus);
    value.dataQualityStatus = std::move(dataQualityStatus);
    value.hasRows = hasRows;
    value.stale = stale;
    value.issues = {
        {"VISIBLE_WARNING", "WARNING", "warning remains visible", false, "presenter-test"},
        {"VISIBLE_BLOCKING", "ERROR", "blocking remains visible", true, "presenter-test"},
    };
    return value;
}

void syncAndExpect(
    etfdt::shell_services::ShellAccountingPresenter& presenter,
    etfdt::shell_services::ShellAccountingReadOnlyController& controller,
    etfdt::shell_services::ShellAccountingStateSnapshot value,
    etfdt::shell_services::ShellAccountingViewState expected,
    const char* message)
{
    controller.applyStateSnapshot(std::move(value));
    presenter.syncFromController();
    expectTrue(presenter.statusObject().state() == expected, message);
    expectTrue(presenter.issueListModel().rowCount() == 2, "issues remain visible through presenter");
    expectTrue(presenter.issueListModel().blockingIssueCount() == 1, "blocking issue count remains visible");
}

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingPresenter;
    using etfdt::shell_services::ShellAccountingReadOnlyController;
    using etfdt::shell_services::ShellAccountingViewState;

    ShellAccountingPresenter presenter;
    auto controller = std::make_shared<ShellAccountingReadOnlyController>();
    presenter.setController(controller);

    expectTrue(presenter.hasController(), "presenter stores controller");
    expectTrue(presenter.issueListModel().rowCount() == 0, "issue count defaults to zero");
    expectTrue(presenter.positionListModel().rowCount() == 0, "position row count defaults to zero");

    syncAndExpect(presenter, *controller, snapshot("", "OK", false, true), ShellAccountingViewState::Unavailable, "implemented=false maps to Unavailable");
    expectTrue(presenter.statusObject().unavailable(), "unavailable flag is visible");
    expectTrue(!presenter.statusObject().empty(), "Unavailable is not Empty");

    syncAndExpect(presenter, *controller, snapshot("POSITION_LIST_NOT_AVAILABLE", "UNAVAILABLE", false, false), ShellAccountingViewState::Unavailable, "position guard maps to Unavailable");
    syncAndExpect(presenter, *controller, snapshot("", "OK", true, true), ShellAccountingViewState::Loaded, "OK with rows maps to Loaded");
    syncAndExpect(presenter, *controller, snapshot("", "OK", true, false), ShellAccountingViewState::Empty, "OK without rows maps to Empty");
    expectTrue(presenter.statusObject().empty(), "empty flag is visible");
    expectTrue(!presenter.statusObject().unavailable(), "Empty is not Unavailable");

    syncAndExpect(presenter, *controller, snapshot("", "WARNING", true, true), ShellAccountingViewState::Warning, "warning remains visible");
    syncAndExpect(presenter, *controller, snapshot("", "ERROR", true, true), ShellAccountingViewState::Error, "error remains visible");
    syncAndExpect(presenter, *controller, snapshot("", "OK", true, true, true), ShellAccountingViewState::Stale, "stale remains visible");

    return 0;
}
