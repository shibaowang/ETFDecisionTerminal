#include "ShellServices/ShellReadOnlyTableProxyModel.h"

#include <QAbstractItemModel>
#include <QVariant>

#include <algorithm>

namespace etfdt::shell_services {
namespace {

bool isNumericVariant(const QVariant& value)
{
    switch (value.userType()) {
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Double:
    case QMetaType::Float:
        return true;
    default:
        return false;
    }
}

}  // namespace

void ShellReadOnlyFilterState::clear()
{
    searchText.clear();
    enabledOnly = false;
    activeOnly = false;
    disabledOnly = false;
    typeFilter.clear();
    marketFilter.clear();
    strategyCodeFilter.clear();
    showOnlyWithValue = false;
}

ShellReadOnlyTableProxyModel::ShellReadOnlyTableProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
    setSortCaseSensitivity(Qt::CaseInsensitive);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void ShellReadOnlyTableProxyModel::setSearchRoleNames(QStringList roleNames)
{
    searchRoleNames_ = std::move(roleNames);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setEnabledRoleName(QString roleName)
{
    enabledRoleName_ = std::move(roleName);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setActiveRoleName(QString roleName)
{
    activeRoleName_ = std::move(roleName);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setTypeRoleName(QString roleName)
{
    typeRoleName_ = std::move(roleName);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setMarketRoleName(QString roleName)
{
    marketRoleName_ = std::move(roleName);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setStrategyCodeRoleName(QString roleName)
{
    strategyCodeRoleName_ = std::move(roleName);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setValueRoleName(QString roleName)
{
    valueRoleName_ = std::move(roleName);
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setSearchText(const QString& text)
{
    const auto trimmed = text.trimmed();
    if (filter_.searchText == trimmed) {
        return;
    }
    filter_.searchText = trimmed;
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setEnabledOnly(bool enabledOnly)
{
    if (filter_.enabledOnly == enabledOnly) {
        return;
    }
    filter_.enabledOnly = enabledOnly;
    if (enabledOnly) {
        filter_.disabledOnly = false;
    }
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setActiveOnly(bool activeOnly)
{
    if (filter_.activeOnly == activeOnly) {
        return;
    }
    filter_.activeOnly = activeOnly;
    if (activeOnly) {
        filter_.disabledOnly = false;
    }
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setDisabledOnly(bool disabledOnly)
{
    if (filter_.disabledOnly == disabledOnly) {
        return;
    }
    filter_.disabledOnly = disabledOnly;
    if (disabledOnly) {
        filter_.enabledOnly = false;
        filter_.activeOnly = false;
    }
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setTypeFilter(const QString& typeFilter)
{
    const auto trimmed = typeFilter.trimmed();
    if (filter_.typeFilter == trimmed) {
        return;
    }
    filter_.typeFilter = trimmed;
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setMarketFilter(const QString& marketFilter)
{
    const auto trimmed = marketFilter.trimmed();
    if (filter_.marketFilter == trimmed) {
        return;
    }
    filter_.marketFilter = trimmed;
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setStrategyCodeFilter(const QString& strategyCodeFilter)
{
    const auto trimmed = strategyCodeFilter.trimmed();
    if (filter_.strategyCodeFilter == trimmed) {
        return;
    }
    filter_.strategyCodeFilter = trimmed;
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::setShowOnlyWithValue(bool showOnlyWithValue)
{
    if (filter_.showOnlyWithValue == showOnlyWithValue) {
        return;
    }
    filter_.showOnlyWithValue = showOnlyWithValue;
    invalidateFilter();
}

void ShellReadOnlyTableProxyModel::clearFilters()
{
    filter_.clear();
    invalidateFilter();
}

bool ShellReadOnlyTableProxyModel::setSortRoleByName(const QString& roleName, bool ascending)
{
    const auto role = roleForName(roleName);
    if (role < 0) {
        return false;
    }
    setSortRole(role);
    sort(0, ascending ? Qt::AscendingOrder : Qt::DescendingOrder);
    return true;
}

const ShellReadOnlyFilterState& ShellReadOnlyTableProxyModel::filterState() const noexcept
{
    return filter_;
}

bool ShellReadOnlyTableProxyModel::filterAcceptsRow(
    int sourceRow,
    const QModelIndex& sourceParent) const
{
    Q_UNUSED(sourceParent);
    if (sourceModel() == nullptr) {
        return false;
    }
    return rowMatchesSearch(sourceRow)
        && rowMatchesBooleanFilter(sourceRow)
        && rowMatchesStringFilter(sourceRow, typeRoleName_, filter_.typeFilter)
        && rowMatchesStringFilter(sourceRow, marketRoleName_, filter_.marketFilter)
        && rowMatchesStringFilter(sourceRow, strategyCodeRoleName_, filter_.strategyCodeFilter)
        && rowHasValue(sourceRow);
}

bool ShellReadOnlyTableProxyModel::lessThan(
    const QModelIndex& left,
    const QModelIndex& right) const
{
    const auto leftValue = sourceModel()->data(left, sortRole());
    const auto rightValue = sourceModel()->data(right, sortRole());

    if (leftValue.userType() == QMetaType::Bool || rightValue.userType() == QMetaType::Bool) {
        return leftValue.toBool() < rightValue.toBool();
    }
    if (isNumericVariant(leftValue) && isNumericVariant(rightValue)) {
        return leftValue.toDouble() < rightValue.toDouble();
    }
    return QString::localeAwareCompare(leftValue.toString(), rightValue.toString()) < 0;
}

int ShellReadOnlyTableProxyModel::roleForName(const QString& roleName) const
{
    if (sourceModel() == nullptr || roleName.isEmpty()) {
        return -1;
    }
    const auto needle = roleName.toUtf8();
    const auto roles = sourceModel()->roleNames();
    for (auto it = roles.cbegin(); it != roles.cend(); ++it) {
        if (it.value() == needle) {
            return it.key();
        }
    }
    return -1;
}

QVariant ShellReadOnlyTableProxyModel::sourceData(int sourceRow, int role) const
{
    if (sourceModel() == nullptr || role < 0) {
        return {};
    }
    return sourceModel()->data(sourceModel()->index(sourceRow, 0), role);
}

bool ShellReadOnlyTableProxyModel::rowMatchesSearch(int sourceRow) const
{
    if (filter_.searchText.isEmpty()) {
        return true;
    }
    const auto roles = searchRoleNames_.isEmpty()
        ? sourceModel()->roleNames().values()
        : QList<QByteArray>{};
    if (searchRoleNames_.isEmpty()) {
        return std::any_of(roles.cbegin(), roles.cend(), [&](const QByteArray& roleName) {
            return sourceData(sourceRow, roleForName(QString::fromUtf8(roleName)))
                .toString()
                .contains(filter_.searchText, Qt::CaseInsensitive);
        });
    }
    return std::any_of(searchRoleNames_.cbegin(), searchRoleNames_.cend(), [&](const QString& roleName) {
        return sourceData(sourceRow, roleForName(roleName))
            .toString()
            .contains(filter_.searchText, Qt::CaseInsensitive);
    });
}

bool ShellReadOnlyTableProxyModel::rowMatchesBooleanFilter(int sourceRow) const
{
    const auto enabledRole = roleForName(enabledRoleName_);
    const auto activeRole = roleForName(activeRoleName_);
    const auto hasEnabled = enabledRole >= 0;
    const auto hasActive = activeRole >= 0;
    const auto enabled = hasEnabled && sourceData(sourceRow, enabledRole).toBool();
    const auto active = hasActive && sourceData(sourceRow, activeRole).toBool();

    if (filter_.enabledOnly && hasEnabled && !enabled) {
        return false;
    }
    if (filter_.activeOnly && hasActive && !active) {
        return false;
    }
    if (filter_.disabledOnly) {
        if (hasEnabled) {
            return !enabled;
        }
        if (hasActive) {
            return !active;
        }
    }
    return true;
}

bool ShellReadOnlyTableProxyModel::rowMatchesStringFilter(
    int sourceRow,
    const QString& roleName,
    const QString& expected) const
{
    if (expected.isEmpty()) {
        return true;
    }
    const auto role = roleForName(roleName);
    if (role < 0) {
        return true;
    }
    return sourceData(sourceRow, role).toString().compare(expected, Qt::CaseInsensitive) == 0;
}

bool ShellReadOnlyTableProxyModel::rowHasValue(int sourceRow) const
{
    if (!filter_.showOnlyWithValue) {
        return true;
    }
    const auto role = roleForName(valueRoleName_);
    if (role < 0) {
        return true;
    }
    return !sourceData(sourceRow, role).toString().trimmed().isEmpty();
}

}  // namespace etfdt::shell_services
