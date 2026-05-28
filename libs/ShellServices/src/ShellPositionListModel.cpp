#include "ShellServices/ShellPositionListModel.h"

#include <QString>
#include <QVariant>

#include <utility>

namespace etfdt::shell_services {

namespace {

bool invalidIndex(const QModelIndex& index, const std::vector<ShellPositionListRow>& rows)
{
    return !index.isValid() || index.row() < 0
        || index.row() >= static_cast<int>(rows.size());
}

}  // namespace

ShellPositionListModel::ShellPositionListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellPositionListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant ShellPositionListModel::data(const QModelIndex& index, int role) const
{
    if (invalidIndex(index, rows_)) {
        return {};
    }

    const auto& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case AccountIdRole: return QString::fromStdString(row.accountId);
    case PortfolioIdRole: return QString::fromStdString(row.portfolioId);
    case InstrumentCodeRole: return QString::fromStdString(row.instrumentCode);
    case InstrumentNameRole: return QString::fromStdString(row.instrumentName);
    case QuantityTextRole: return QString::fromStdString(row.quantityText);
    case CostAmountDisplayTextRole:
        return QString::fromStdString(shellAccountingDisplayText(
            row.costAmountText,
            privacyMode_,
            row.costAmountState));
    case CostPriceDisplayTextRole:
        return QString::fromStdString(shellAccountingDisplayText(
            row.costPriceText,
            privacyMode_,
            row.costPriceState));
    case CurrencyRole: return QString::fromStdString(row.currency);
    case MarketValueDisplayTextRole:
        return QString::fromStdString(shellAccountingDisplayText(
            row.marketValueText,
            privacyMode_,
            row.marketValueState));
    case UnrealizedPnlDisplayTextRole:
        return QString::fromStdString(shellAccountingDisplayText(
            row.unrealizedPnlText,
            privacyMode_,
            row.unrealizedPnlState));
    case DataQualityStatusRole: return QString::fromStdString(row.dataQualityStatus);
    default: return {};
    }
}

QHash<int, QByteArray> ShellPositionListModel::roleNames() const
{
    return {
        {AccountIdRole, "accountId"},
        {PortfolioIdRole, "portfolioId"},
        {InstrumentCodeRole, "instrumentCode"},
        {InstrumentNameRole, "instrumentName"},
        {QuantityTextRole, "quantityText"},
        {CostAmountDisplayTextRole, "costAmountDisplayText"},
        {CostPriceDisplayTextRole, "costPriceDisplayText"},
        {CurrencyRole, "currency"},
        {MarketValueDisplayTextRole, "marketValueDisplayText"},
        {UnrealizedPnlDisplayTextRole, "unrealizedPnlDisplayText"},
        {DataQualityStatusRole, "dataQualityStatus"},
    };
}

void ShellPositionListModel::setRows(std::vector<ShellPositionListRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

void ShellPositionListModel::clearRows()
{
    setRows(std::vector<ShellPositionListRow>{});
}

void ShellPositionListModel::setPrivacyMode(bool enabled)
{
    if (privacyMode_ == enabled) {
        return;
    }
    privacyMode_ = enabled;
    if (!rows_.empty()) {
        const auto topLeft = index(0, 0);
        const auto bottomRight = index(static_cast<int>(rows_.size()) - 1, 0);
        emit dataChanged(
            topLeft,
            bottomRight,
            {CostAmountDisplayTextRole,
             CostPriceDisplayTextRole,
             MarketValueDisplayTextRole,
             UnrealizedPnlDisplayTextRole});
    }
}

bool ShellPositionListModel::privacyMode() const noexcept
{
    return privacyMode_;
}

const std::vector<ShellPositionListRow>& ShellPositionListModel::rows() const noexcept
{
    return rows_;
}

}  // namespace etfdt::shell_services
