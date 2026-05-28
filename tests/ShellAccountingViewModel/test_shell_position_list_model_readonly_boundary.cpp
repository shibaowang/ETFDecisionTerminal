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

etfdt::shell_services::ShellPositionListRow makeRow()
{
    using etfdt::shell_services::ShellAccountingViewState;
    etfdt::shell_services::ShellPositionListRow row;
    row.accountId = "ACC-001";
    row.portfolioId = "PF-001";
    row.instrumentCode = "159509";
    row.instrumentName = "ETF Sample";
    row.quantityText = "100.0000";
    row.costAmountText = {"1000.00 CNY", "***"};
    row.costPriceText = {"10.00 CNY", "***"};
    row.currency = "CNY";
    row.marketValueText = {"", "***"};
    row.marketValueState = ShellAccountingViewState::Unavailable;
    row.unrealizedPnlText = {"", "***"};
    row.unrealizedPnlState = ShellAccountingViewState::Unavailable;
    row.dataQualityStatus = "UNAVAILABLE";
    return row;
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    etfdt::shell_services::ShellPositionListModel model;
    expectTrue(model.rowCount() == 0, "position list model defaults to empty");

    model.setRows({makeRow()});
    expectTrue(model.rowCount() == 1, "test-only row can be loaded");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellPositionListModel::AccountIdRole), "accountId role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellPositionListModel::PortfolioIdRole), "portfolioId role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellPositionListModel::InstrumentCodeRole), "instrumentCode role exists");
    expectTrue(model.roleNames().contains(etfdt::shell_services::ShellPositionListModel::QuantityTextRole), "quantityText role exists");

    const QModelIndex row = model.index(0, 0);
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::AccountIdRole)) == "ACC-001", "accountId role is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::PortfolioIdRole)) == "PF-001", "portfolioId role is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::InstrumentCodeRole)) == "159509", "instrumentCode role is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::InstrumentNameRole)) == "ETF Sample", "instrumentName role is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::QuantityTextRole)) == "100.0000", "quantityText role is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::CostAmountDisplayTextRole)) == "1000.00 CNY", "cost amount displayText is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::CostPriceDisplayTextRole)) == "10.00 CNY", "cost price displayText is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::CurrencyRole)) == "CNY", "currency role is correct");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::MarketValueDisplayTextRole)).empty(), "unavailable market value does not fake amount");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::UnrealizedPnlDisplayTextRole)).empty(), "unavailable unrealized PnL does not fake amount");
    expectTrue(asString(model.data(row, etfdt::shell_services::ShellPositionListModel::DataQualityStatusRole)) == "UNAVAILABLE", "dataQualityStatus role is correct");

    model.clearRows();
    expectTrue(model.rowCount() == 0, "clearRows empties model");

    return 0;
}
