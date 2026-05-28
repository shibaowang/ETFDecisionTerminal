#include "ShellServices/ShellAccountingIssueListModel.h"
#include "ShellServices/ShellPositionListModel.h"

#include <QCoreApplication>

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

std::string asString(const QVariant& value)
{
    return value.toString().toStdString();
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    using etfdt::shell_services::ShellAccountingViewState;
    etfdt::shell_services::ShellPositionListRow row;
    row.accountId = "ACC-001";
    row.portfolioId = "PF-001";
    row.instrumentCode = "159509";
    row.quantityText = "100.0000";
    row.costAmountText = {"1000.00 CNY", "***"};
    row.costPriceText = {"10.00 CNY", "***"};
    row.marketValueText = {"", "***"};
    row.marketValueState = ShellAccountingViewState::Unavailable;
    row.unrealizedPnlText = {"", "***"};
    row.unrealizedPnlState = ShellAccountingViewState::Empty;

    etfdt::shell_services::ShellPositionListModel model;
    model.setRows({row});
    const QModelIndex index = model.index(0, 0);

    expectTrue(asString(model.data(index, etfdt::shell_services::ShellPositionListModel::CostAmountDisplayTextRole)) == "1000.00 CNY", "privacyMode=false returns rawText");
    model.setPrivacyMode(true);
    expectTrue(asString(model.data(index, etfdt::shell_services::ShellPositionListModel::CostAmountDisplayTextRole)) == "***", "privacyMode=true returns maskedText");
    expectTrue(model.rows().front().costAmountText.rawText == "1000.00 CNY", "privacy mode does not mutate rawText");
    expectTrue(asString(model.data(index, etfdt::shell_services::ShellPositionListModel::MarketValueDisplayTextRole)).empty(), "Unavailable does not fake masked amount");
    expectTrue(asString(model.data(index, etfdt::shell_services::ShellPositionListModel::UnrealizedPnlDisplayTextRole)).empty(), "Empty does not fake amount");

    etfdt::shell_services::ShellAccountingIssueListModel issues;
    issues.setIssues({{"MARKET_PRICE_MISSING", "WARNING", "Price missing", false, "position.list"}});
    expectTrue(issues.rowCount() == 1, "issue remains visible in privacy mode");
    expectTrue(asString(issues.data(issues.index(0, 0), etfdt::shell_services::ShellAccountingIssueListModel::CodeRole)) == "MARKET_PRICE_MISSING", "issue code remains visible in privacy mode");

    return 0;
}
