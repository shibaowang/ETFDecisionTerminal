#include "ShellServices/ShellAccountingPresenter.h"

#include <cstdlib>
#include <iostream>
#include <memory>

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
    using etfdt::shell_services::ShellAccountingReadOnlyController;
    using etfdt::shell_services::ShellAccountingViewState;

    ShellAccountingPresenter presenter;
    presenter.refreshPositionList();
    expectTrue(presenter.statusObject().state() == ShellAccountingViewState::Unavailable, "refresh without controller maps to Unavailable");
    expectTrue(presenter.issueListModel().rowCount() == 1, "refresh without controller exposes issue");
    expectTrue(presenter.issueListModel().issues().front().code == "CONTROLLER_NOT_CONFIGURED", "controller missing issue code is preserved");
    expectTrue(presenter.issueListModel().blockingIssueCount() == 1, "controller missing issue is blocking");
    expectTrue(presenter.readOnly(), "presenter remains read-only after missing controller refresh");
    expectTrue(!presenter.writeEnabled(), "presenter remains write disabled after missing controller refresh");
    expectTrue(presenter.positionListModel().rowCount() == 0, "refresh without controller does not create positions");

    auto controller = std::make_shared<ShellAccountingReadOnlyController>();
    presenter.setController(controller);
    expectTrue(presenter.hasController(), "controller can be injected");
    presenter.refreshPositionList();
    expectTrue(presenter.statusObject().state() == ShellAccountingViewState::Unavailable, "controller refresh without service adapter remains controlled unavailable");
    expectTrue(presenter.issueListModel().rowCount() == 1, "controller refresh issue remains visible");
    expectTrue(presenter.issueListModel().issues().front().code == "SERVICE_ADAPTER_NOT_CONFIGURED", "presenter only reports controller boundary issue");

    presenter.clearController();
    expectTrue(!presenter.hasController(), "controller can be cleared");
    presenter.refreshAllReadOnly();
    expectTrue(presenter.statusObject().state() == ShellAccountingViewState::Unavailable, "refreshAllReadOnly is controlled without controller");
    expectTrue(presenter.issueListModel().issues().front().code == "CONTROLLER_NOT_CONFIGURED", "refreshAllReadOnly uses controller boundary");

    return 0;
}
