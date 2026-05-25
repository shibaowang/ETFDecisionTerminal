#pragma once

#include <QAbstractListModel>

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::shell_services {

struct ShellAccountRow final {
    std::int64_t id = 0;
    std::string uid;
    std::string name;
    std::string accountType;
    std::string brokerName;
    std::string baseCurrency;
    bool isActive = false;
    std::string initialCashText;
};

struct ShellPortfolioRow final {
    std::int64_t id = 0;
    std::string uid;
    std::string name;
    std::string basePositionRatioText;
    bool isActive = false;
};

struct ShellInstrumentRow final {
    std::int64_t id = 0;
    std::string uid;
    std::string code;
    std::string name;
    std::string instrumentType;
    std::string marketCode;
    std::string currency;
    bool enabled = false;
};

struct ShellStrategyRow final {
    std::int64_t id = 0;
    std::string uid;
    std::string strategyCode;
    std::string name;
    bool enabled = false;
};

struct ShellOtcChannelRow final {
    std::int64_t id = 0;
    std::string uid;
    std::string strategyCode;
    std::string actualCode;
    std::string fundClass;
    bool enabled = false;
    std::string dailyLimitText;
    int priority = 0;
    std::string minBuyAmountText;
};

class ShellAccountListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        UidRole,
        NameRole,
        AccountTypeRole,
        BrokerNameRole,
        BaseCurrencyRole,
        IsActiveRole,
        InitialCashTextRole,
    };

    explicit ShellAccountListModel(QObject* parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    void setRows(std::vector<ShellAccountRow> rows);

private:
    std::vector<ShellAccountRow> rows_;
};

class ShellPortfolioListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        UidRole,
        NameRole,
        BasePositionRatioTextRole,
        IsActiveRole,
    };

    explicit ShellPortfolioListModel(QObject* parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    void setRows(std::vector<ShellPortfolioRow> rows);

private:
    std::vector<ShellPortfolioRow> rows_;
};

class ShellInstrumentListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        UidRole,
        CodeRole,
        NameRole,
        InstrumentTypeRole,
        MarketCodeRole,
        CurrencyRole,
        EnabledRole,
    };

    explicit ShellInstrumentListModel(QObject* parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    void setRows(std::vector<ShellInstrumentRow> rows);

private:
    std::vector<ShellInstrumentRow> rows_;
};

class ShellStrategyListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        UidRole,
        StrategyCodeRole,
        NameRole,
        EnabledRole,
    };

    explicit ShellStrategyListModel(QObject* parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    void setRows(std::vector<ShellStrategyRow> rows);

private:
    std::vector<ShellStrategyRow> rows_;
};

class ShellOtcChannelListModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        IdRole = Qt::UserRole + 1,
        UidRole,
        StrategyCodeRole,
        ActualCodeRole,
        FundClassRole,
        EnabledRole,
        DailyLimitTextRole,
        PriorityRole,
        MinBuyAmountTextRole,
    };

    explicit ShellOtcChannelListModel(QObject* parent = nullptr);
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    void setRows(std::vector<ShellOtcChannelRow> rows);
    void clearRows();

private:
    std::vector<ShellOtcChannelRow> rows_;
};

}  // namespace etfdt::shell_services
