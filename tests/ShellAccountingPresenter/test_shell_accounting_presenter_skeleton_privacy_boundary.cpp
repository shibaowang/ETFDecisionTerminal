#include "ShellServices/ShellAccountingPresenter.h"

#include <QCoreApplication>
#include <QModelIndex>

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

std::string roleText(
    etfdt::shell_services::ShellPositionListModel& model,
    int role)
{
    return model.data(model.index(0, 0), role).toString().toStdString();
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    using etfdt::shell_services::ShellAccountingIssue;
    using etfdt::shell_services::ShellAccountingPresenter;
    using etfdt::shell_services::ShellAccountingViewState;
    using etfdt::shell_services::ShellPositionListModel;
    using etfdt::shell_services::ShellPositionListRow;

    ShellAccountingPresenter presenter;
    ShellPositionListRow row;
    row.accountId = "acct-1";
    row.portfolioId = "portfolio-1";
    row.instrumentCode = "ETF-001";
    row.quantityText = "10";
    row.costAmountText = {"123.45", "***"};
    row.marketValueText = {"", "***"};
    row.marketValueState = ShellAccountingViewState::Unavailable;
    presenter.positionListModel().setRows({row});

    expectTrue(!presenter.privacyMode(), "privacyMode defaults to false");
    expectTrue(roleText(presenter.positionListModel(), ShellPositionListModel::CostAmountDisplayTextRole) == "123.45", "privacy=false displays raw text");
    expectTrue(roleText(presenter.positionListModel(), ShellPositionListModel::MarketValueDisplayTextRole).empty(), "Unavailable amount is not fabricated");

    presenter.setPrivacyMode(true);
    expectTrue(presenter.privacyMode(), "privacyMode=true is stored");
    expectTrue(presenter.positionListModel().privacyMode(), "privacy mode is forwarded to position model");
    expectTrue(roleText(presenter.positionListModel(), ShellPositionListModel::CostAmountDisplayTextRole) == "***", "privacy=true displays masked text");
    expectTrue(row.costAmountText.rawText == "123.45", "raw text object used by test remains unchanged");

    presenter.issueListModel().setIssues({
        ShellAccountingIssue{"VISIBLE_WARNING", "WARNING", "warning remains visible", false, "privacy-test"},
    });
    expectTrue(presenter.issueListModel().rowCount() == 1, "issues remain visible under privacy");

    presenter.setPrivacyMode(false);
    expectTrue(!presenter.privacyMode(), "privacyMode=false is stored");
    expectTrue(roleText(presenter.positionListModel(), ShellPositionListModel::CostAmountDisplayTextRole) == "123.45", "privacy=false restores raw display text");

    return 0;
}
