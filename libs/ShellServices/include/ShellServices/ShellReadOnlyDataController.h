#pragma once

#include "ShellServices/ShellDataConnectionObject.h"
#include "ShellServices/ShellReadOnlyConnectionPresets.h"
#include "ShellServices/ShellReadOnlyDataFacade.h"
#include "ShellServices/ShellReadOnlyDataViewModel.h"
#include "ShellServices/ShellReadOnlyListModels.h"
#include "ShellServices/ShellReadOnlyTableProxyModel.h"

#include <QObject>
#include <QAbstractItemModel>
#include <QString>
#include <QVariantMap>
#include <optional>
#include <string>

namespace etfdt::shell_services {

class ShellReadOnlyDataController : public QObject {
    Q_OBJECT
    Q_PROPERTY(ShellDataConnectionObject* connectionObject READ connectionObject CONSTANT)
    Q_PROPERTY(QVariantMap summaryViewModel READ summaryViewModelMap NOTIFY summaryChanged)
    Q_PROPERTY(ShellAccountListModel* accountModel READ accountModel CONSTANT)
    Q_PROPERTY(ShellPortfolioListModel* portfolioModel READ portfolioModel CONSTANT)
    Q_PROPERTY(ShellInstrumentListModel* instrumentModel READ instrumentModel CONSTANT)
    Q_PROPERTY(ShellStrategyListModel* strategyModel READ strategyModel CONSTANT)
    Q_PROPERTY(ShellOtcChannelListModel* otcChannelModel READ otcChannelModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* filteredAccountModel READ filteredAccountItemModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* filteredPortfolioModel READ filteredPortfolioItemModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* filteredInstrumentModel READ filteredInstrumentItemModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* filteredStrategyModel READ filteredStrategyItemModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* filteredOtcChannelModel READ filteredOtcChannelItemModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* connectionPresetModel READ connectionPresetItemModel CONSTANT)
    Q_PROPERTY(QString lastError READ lastErrorText NOTIFY lastErrorChanged)
    Q_PROPERTY(QString selectedPresetKey READ selectedPresetKey NOTIFY connectionPresetChanged)
    Q_PROPERTY(QString selectedSocketName READ selectedSocketName WRITE setCustomSocketName NOTIFY connectionPresetChanged)
    Q_PROPERTY(QString commandHint READ commandHint NOTIFY connectionPresetChanged)
    Q_PROPERTY(QString onboardingText READ onboardingText CONSTANT)
    Q_PROPERTY(QString refreshState READ refreshState NOTIFY stateChanged)
    Q_PROPERTY(bool isBusy READ isBusy NOTIFY stateChanged)
    Q_PROPERTY(bool isConnecting READ isConnecting NOTIFY stateChanged)
    Q_PROPERTY(bool isRefreshing READ isRefreshing NOTIFY stateChanged)
    Q_PROPERTY(bool canRefresh READ canRefresh NOTIFY stateChanged)
    Q_PROPERTY(QString lastRefreshStartedAtText READ lastRefreshStartedAtText NOTIFY stateChanged)
    Q_PROPERTY(QString lastRefreshFinishedAtText READ lastRefreshFinishedAtText NOTIFY stateChanged)
    Q_PROPERTY(QString lastSuccessAtText READ lastSuccessAtText NOTIFY stateChanged)
    Q_PROPERTY(QString selectedStrategyCode READ selectedStrategyCode WRITE setSelectedStrategyCode NOTIFY selectedStrategyCodeChanged)
    Q_PROPERTY(QVariantMap errorState READ errorStateMap NOTIFY errorStateChanged)
    Q_PROPERTY(int refreshAttemptCount READ refreshAttemptCount NOTIFY stateChanged)
    Q_PROPERTY(int refreshSuccessCount READ refreshSuccessCount NOTIFY stateChanged)
    Q_PROPERTY(int refreshFailureCount READ refreshFailureCount NOTIFY stateChanged)
    Q_PROPERTY(int refreshThrottleCount READ refreshThrottleCount NOTIFY stateChanged)

public:
    explicit ShellReadOnlyDataController(QObject* parent = nullptr);

    [[nodiscard]] ShellDataResult<bool> connectToDataService(
        const std::string& socketName,
        int timeoutMs);
    Q_INVOKABLE [[nodiscard]] bool connectToDataService(const QString& socketName);
    Q_INVOKABLE [[nodiscard]] bool connectToDataService();
    Q_INVOKABLE void disconnect();

    [[nodiscard]] ShellDataResult<bool> refreshHealth(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshSummary(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshAccounts(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshPortfolios(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshAccountsAndPortfolios(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshInstruments(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshStrategies(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshInstrumentsAndStrategies(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshOtcChannels(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshAll(int timeoutMs);

    Q_INVOKABLE [[nodiscard]] bool refreshHealth();
    Q_INVOKABLE [[nodiscard]] bool refreshSummary();
    Q_INVOKABLE [[nodiscard]] bool refreshAccounts();
    Q_INVOKABLE [[nodiscard]] bool refreshPortfolios();
    Q_INVOKABLE [[nodiscard]] bool refreshAccountsAndPortfolios();
    Q_INVOKABLE [[nodiscard]] bool refreshInstruments();
    Q_INVOKABLE [[nodiscard]] bool refreshStrategies();
    Q_INVOKABLE [[nodiscard]] bool refreshInstrumentsAndStrategies();
    Q_INVOKABLE [[nodiscard]] bool refreshOtcChannels();
    Q_INVOKABLE [[nodiscard]] bool refreshAll();
    Q_INVOKABLE [[nodiscard]] bool refreshOtc(const QString& strategyCode);
    Q_INVOKABLE void clearOtcChannels();
    Q_INVOKABLE [[nodiscard]] bool selectPreset(const QString& key);
    Q_INVOKABLE void setCustomSocketName(const QString& socketName);
    Q_INVOKABLE void setSelectedStrategyCode(const QString& strategyCode);
    Q_INVOKABLE void setAccountSearchText(const QString& text);
    Q_INVOKABLE void setPortfolioSearchText(const QString& text);
    Q_INVOKABLE void setInstrumentSearchText(const QString& text);
    Q_INVOKABLE void setStrategySearchText(const QString& text);
    Q_INVOKABLE void setOtcSearchText(const QString& text);
    Q_INVOKABLE void setAccountActiveOnly(bool activeOnly);
    Q_INVOKABLE void setPortfolioActiveOnly(bool activeOnly);
    Q_INVOKABLE void setInstrumentEnabledOnly(bool enabledOnly);
    Q_INVOKABLE void setStrategyEnabledOnly(bool enabledOnly);
    Q_INVOKABLE void setOtcEnabledOnly(bool enabledOnly);
    Q_INVOKABLE bool sortAccounts(const QString& key, bool ascending);
    Q_INVOKABLE bool sortPortfolios(const QString& key, bool ascending);
    Q_INVOKABLE bool sortInstruments(const QString& key, bool ascending);
    Q_INVOKABLE bool sortStrategies(const QString& key, bool ascending);
    Q_INVOKABLE bool sortOtcChannels(const QString& key, bool ascending);
    Q_INVOKABLE void clearReadonlyFilters();

    [[nodiscard]] ShellDataConnectionObject* connectionObject();
    [[nodiscard]] const ShellReadOnlyDataViewModel& summaryViewModel() const noexcept;
    [[nodiscard]] QVariantMap summaryViewModelMap() const;
    [[nodiscard]] ShellAccountListModel* accountModel();
    [[nodiscard]] ShellPortfolioListModel* portfolioModel();
    [[nodiscard]] ShellInstrumentListModel* instrumentModel();
    [[nodiscard]] ShellStrategyListModel* strategyModel();
    [[nodiscard]] ShellOtcChannelListModel* otcChannelModel();
    [[nodiscard]] QAbstractItemModel* filteredAccountItemModel();
    [[nodiscard]] QAbstractItemModel* filteredPortfolioItemModel();
    [[nodiscard]] QAbstractItemModel* filteredInstrumentItemModel();
    [[nodiscard]] QAbstractItemModel* filteredStrategyItemModel();
    [[nodiscard]] QAbstractItemModel* filteredOtcChannelItemModel();
    [[nodiscard]] ShellReadOnlyTableProxyModel* filteredAccountModel();
    [[nodiscard]] ShellReadOnlyTableProxyModel* filteredPortfolioModel();
    [[nodiscard]] ShellReadOnlyTableProxyModel* filteredInstrumentModel();
    [[nodiscard]] ShellReadOnlyTableProxyModel* filteredStrategyModel();
    [[nodiscard]] ShellReadOnlyTableProxyModel* filteredOtcChannelModel();
    [[nodiscard]] QAbstractItemModel* connectionPresetItemModel();
    [[nodiscard]] ShellReadOnlyConnectionPresetModel* connectionPresetModel();
    [[nodiscard]] const std::string& lastError() const noexcept;
    [[nodiscard]] QString lastErrorText() const;
    [[nodiscard]] QString selectedPresetKey() const;
    [[nodiscard]] QString selectedSocketName() const;
    [[nodiscard]] QString commandHint() const;
    [[nodiscard]] QString onboardingText() const;
    [[nodiscard]] QString refreshState() const;
    [[nodiscard]] bool isBusy() const noexcept;
    [[nodiscard]] bool isConnecting() const;
    [[nodiscard]] bool isRefreshing() const;
    [[nodiscard]] bool canRefresh() const;
    [[nodiscard]] QString lastRefreshStartedAtText() const;
    [[nodiscard]] QString lastRefreshFinishedAtText() const;
    [[nodiscard]] QString lastSuccessAtText() const;
    [[nodiscard]] QString selectedStrategyCode() const;
    [[nodiscard]] QVariantMap errorStateMap() const;
    [[nodiscard]] int refreshAttemptCount() const noexcept;
    [[nodiscard]] int refreshSuccessCount() const noexcept;
    [[nodiscard]] int refreshFailureCount() const noexcept;
    [[nodiscard]] int refreshThrottleCount() const noexcept;
    void setMinimumRefreshIntervalMs(int intervalMs) noexcept;

signals:
    void summaryChanged();
    void lastErrorChanged();
    void connectionPresetChanged();
    void stateChanged();
    void errorStateChanged();
    void selectedStrategyCodeChanged();

private:
    [[nodiscard]] ShellDataResult<bool> refreshAllUnprotected(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> failureFromResponse(
        const ShellDataResponse& response,
        const char* fallbackMessage);
    [[nodiscard]] ShellDataResult<bool> failureFromError(
        const ShellDataError& error,
        const char* fallbackMessage);
    [[nodiscard]] bool isRefreshThrottled() const;
    [[nodiscard]] int remainingThrottleMs() const;
    [[nodiscard]] QString utcNowText() const;
    void setRefreshState(QString state);
    void setBusy(bool busy);
    void setError(
        std::optional<etfdt::protocol::ErrorCode> errorCode,
        std::string message,
        QString source,
        bool recoverable);
    void setLocalError(QString code, std::string message, QString source, bool recoverable);
    void setLastError(std::string message);
    void clearError();
    void notifySummaryChanged();
    void scheduleCanRefreshUpdate();
    [[nodiscard]] QString roleNameForSortKey(const QString& modelName, const QString& key) const;

    ShellReadOnlyDataFacade facade_;
    ShellDataConnectionObject connection_;
    ShellReadOnlyDataViewModel summary_;
    ShellAccountListModel accounts_;
    ShellPortfolioListModel portfolios_;
    ShellInstrumentListModel instruments_;
    ShellStrategyListModel strategies_;
    ShellOtcChannelListModel otcChannels_;
    ShellReadOnlyTableProxyModel filteredAccounts_;
    ShellReadOnlyTableProxyModel filteredPortfolios_;
    ShellReadOnlyTableProxyModel filteredInstruments_;
    ShellReadOnlyTableProxyModel filteredStrategies_;
    ShellReadOnlyTableProxyModel filteredOtcChannels_;
    ShellReadOnlyConnectionPresetModel connectionPresets_;
    std::string lastError_;
    QString selectedPresetKey_ = QStringLiteral("readonly_default");
    QString selectedSocketName_ = QStringLiteral("ETFDataServiceReadonly");
    QString commandHint_ = QStringLiteral("ETFDataService --serve-readonly --db data/ETFDecision.db --socket-name ETFDataServiceReadonly");
    QString refreshState_ = QStringLiteral("IDLE");
    bool isBusy_ = false;
    int minimumRefreshIntervalMs_ = 1000;
    QString lastRefreshStartedAtText_;
    QString lastRefreshFinishedAtText_;
    QString lastSuccessAtText_;
    QString selectedStrategyCode_;
    bool hasError_ = false;
    QString errorCode_;
    QString errorMessage_;
    QString errorSource_;
    bool errorRecoverable_ = true;
    int refreshAttemptCount_ = 0;
    int refreshSuccessCount_ = 0;
    int refreshFailureCount_ = 0;
    int refreshThrottleCount_ = 0;
};

}  // namespace etfdt::shell_services
