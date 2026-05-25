#pragma once

#include <QSortFilterProxyModel>
#include <QString>
#include <QStringList>

namespace etfdt::shell_services {

enum class ShellReadOnlySortKey {
    Name,
    Code,
    Status,
    Enabled,
    Type,
    Market,
    Currency,
    Priority,
    Amount,
};

struct ShellReadOnlyFilterState final {
    QString searchText;
    bool enabledOnly = false;
    bool activeOnly = false;
    bool disabledOnly = false;
    QString typeFilter;
    QString marketFilter;
    QString strategyCodeFilter;
    bool showOnlyWithValue = false;

    void clear();
};

class ShellReadOnlyTableProxyModel final : public QSortFilterProxyModel {
    Q_OBJECT

public:
    explicit ShellReadOnlyTableProxyModel(QObject* parent = nullptr);

    void setSearchRoleNames(QStringList roleNames);
    void setEnabledRoleName(QString roleName);
    void setActiveRoleName(QString roleName);
    void setTypeRoleName(QString roleName);
    void setMarketRoleName(QString roleName);
    void setStrategyCodeRoleName(QString roleName);
    void setValueRoleName(QString roleName);

    Q_INVOKABLE void setSearchText(const QString& text);
    Q_INVOKABLE void setEnabledOnly(bool enabledOnly);
    Q_INVOKABLE void setActiveOnly(bool activeOnly);
    Q_INVOKABLE void setDisabledOnly(bool disabledOnly);
    Q_INVOKABLE void setTypeFilter(const QString& typeFilter);
    Q_INVOKABLE void setMarketFilter(const QString& marketFilter);
    Q_INVOKABLE void setStrategyCodeFilter(const QString& strategyCodeFilter);
    Q_INVOKABLE void setShowOnlyWithValue(bool showOnlyWithValue);
    Q_INVOKABLE void clearFilters();
    Q_INVOKABLE bool setSortRoleByName(const QString& roleName, bool ascending);

    [[nodiscard]] const ShellReadOnlyFilterState& filterState() const noexcept;

protected:
    [[nodiscard]] bool filterAcceptsRow(
        int sourceRow,
        const QModelIndex& sourceParent) const override;
    [[nodiscard]] bool lessThan(
        const QModelIndex& left,
        const QModelIndex& right) const override;

private:
    [[nodiscard]] int roleForName(const QString& roleName) const;
    [[nodiscard]] QVariant sourceData(int sourceRow, int role) const;
    [[nodiscard]] bool rowMatchesSearch(int sourceRow) const;
    [[nodiscard]] bool rowMatchesBooleanFilter(int sourceRow) const;
    [[nodiscard]] bool rowMatchesStringFilter(
        int sourceRow,
        const QString& roleName,
        const QString& expected) const;
    [[nodiscard]] bool rowHasValue(int sourceRow) const;

    ShellReadOnlyFilterState filter_;
    QStringList searchRoleNames_;
    QString enabledRoleName_;
    QString activeRoleName_;
    QString typeRoleName_;
    QString marketRoleName_;
    QString strategyCodeRoleName_;
    QString valueRoleName_;
};

}  // namespace etfdt::shell_services
