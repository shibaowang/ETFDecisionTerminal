#include "ShellServices/ShellReadOnlyListModels.h"

#include <QString>
#include <QVariant>

#include <utility>

namespace etfdt::shell_services {

namespace {

template <typename Row>
bool invalidIndex(const QModelIndex& index, const std::vector<Row>& rows)
{
    return !index.isValid() || index.row() < 0
        || index.row() >= static_cast<int>(rows.size());
}

}  // namespace

ShellAccountListModel::ShellAccountListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellAccountListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant ShellAccountListModel::data(const QModelIndex& index, int role) const
{
    if (invalidIndex(index, rows_)) {
        return {};
    }
    const auto& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case IdRole: return QVariant::fromValue<qlonglong>(row.id);
    case UidRole: return QString::fromStdString(row.uid);
    case NameRole: return QString::fromStdString(row.name);
    case AccountTypeRole: return QString::fromStdString(row.accountType);
    case BrokerNameRole: return QString::fromStdString(row.brokerName);
    case BaseCurrencyRole: return QString::fromStdString(row.baseCurrency);
    case IsActiveRole: return row.isActive;
    case InitialCashTextRole: return QString::fromStdString(row.initialCashText);
    default: return {};
    }
}

QHash<int, QByteArray> ShellAccountListModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {UidRole, "uid"},
        {NameRole, "name"},
        {AccountTypeRole, "accountType"},
        {BrokerNameRole, "brokerName"},
        {BaseCurrencyRole, "baseCurrency"},
        {IsActiveRole, "isActive"},
        {InitialCashTextRole, "initialCashText"},
    };
}

void ShellAccountListModel::setRows(std::vector<ShellAccountRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

ShellPortfolioListModel::ShellPortfolioListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellPortfolioListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant ShellPortfolioListModel::data(const QModelIndex& index, int role) const
{
    if (invalidIndex(index, rows_)) {
        return {};
    }
    const auto& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case IdRole: return QVariant::fromValue<qlonglong>(row.id);
    case UidRole: return QString::fromStdString(row.uid);
    case NameRole: return QString::fromStdString(row.name);
    case BasePositionRatioTextRole: return QString::fromStdString(row.basePositionRatioText);
    case IsActiveRole: return row.isActive;
    default: return {};
    }
}

QHash<int, QByteArray> ShellPortfolioListModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {UidRole, "uid"},
        {NameRole, "name"},
        {BasePositionRatioTextRole, "basePositionRatioText"},
        {IsActiveRole, "isActive"},
    };
}

void ShellPortfolioListModel::setRows(std::vector<ShellPortfolioRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

ShellInstrumentListModel::ShellInstrumentListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellInstrumentListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant ShellInstrumentListModel::data(const QModelIndex& index, int role) const
{
    if (invalidIndex(index, rows_)) {
        return {};
    }
    const auto& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case IdRole: return QVariant::fromValue<qlonglong>(row.id);
    case UidRole: return QString::fromStdString(row.uid);
    case CodeRole: return QString::fromStdString(row.code);
    case NameRole: return QString::fromStdString(row.name);
    case InstrumentTypeRole: return QString::fromStdString(row.instrumentType);
    case MarketCodeRole: return QString::fromStdString(row.marketCode);
    case CurrencyRole: return QString::fromStdString(row.currency);
    case EnabledRole: return row.enabled;
    default: return {};
    }
}

QHash<int, QByteArray> ShellInstrumentListModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {UidRole, "uid"},
        {CodeRole, "code"},
        {NameRole, "name"},
        {InstrumentTypeRole, "instrumentType"},
        {MarketCodeRole, "marketCode"},
        {CurrencyRole, "currency"},
        {EnabledRole, "enabled"},
    };
}

void ShellInstrumentListModel::setRows(std::vector<ShellInstrumentRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

ShellStrategyListModel::ShellStrategyListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellStrategyListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(rows_.size());
}

QVariant ShellStrategyListModel::data(const QModelIndex& index, int role) const
{
    if (invalidIndex(index, rows_)) {
        return {};
    }
    const auto& row = rows_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case IdRole: return QVariant::fromValue<qlonglong>(row.id);
    case UidRole: return QString::fromStdString(row.uid);
    case StrategyCodeRole: return QString::fromStdString(row.strategyCode);
    case NameRole: return QString::fromStdString(row.name);
    case EnabledRole: return row.enabled;
    default: return {};
    }
}

QHash<int, QByteArray> ShellStrategyListModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {UidRole, "uid"},
        {StrategyCodeRole, "strategyCode"},
        {NameRole, "name"},
        {EnabledRole, "enabled"},
    };
}

void ShellStrategyListModel::setRows(std::vector<ShellStrategyRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

}  // namespace etfdt::shell_services
