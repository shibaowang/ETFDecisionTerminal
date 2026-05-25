#pragma once

#include "ShellServices/ShellDataConnectionObject.h"
#include "ShellServices/ShellReadOnlyDataFacade.h"
#include "ShellServices/ShellReadOnlyDataViewModel.h"
#include "ShellServices/ShellReadOnlyListModels.h"

#include <QObject>
#include <QString>
#include <QVariantMap>
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
    Q_PROPERTY(QString lastError READ lastErrorText NOTIFY lastErrorChanged)

public:
    explicit ShellReadOnlyDataController(QObject* parent = nullptr);

    [[nodiscard]] ShellDataResult<bool> connectToDataService(
        const std::string& socketName,
        int timeoutMs);
    Q_INVOKABLE [[nodiscard]] bool connectToDataService(const QString& socketName);
    Q_INVOKABLE void disconnect();

    [[nodiscard]] ShellDataResult<bool> refreshHealth(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshSummary(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshAccounts(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshPortfolios(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshInstruments(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshStrategies(int timeoutMs);
    [[nodiscard]] ShellDataResult<bool> refreshAll(int timeoutMs);

    Q_INVOKABLE [[nodiscard]] bool refreshHealth();
    Q_INVOKABLE [[nodiscard]] bool refreshSummary();
    Q_INVOKABLE [[nodiscard]] bool refreshAccounts();
    Q_INVOKABLE [[nodiscard]] bool refreshPortfolios();
    Q_INVOKABLE [[nodiscard]] bool refreshInstruments();
    Q_INVOKABLE [[nodiscard]] bool refreshStrategies();
    Q_INVOKABLE [[nodiscard]] bool refreshAll();

    [[nodiscard]] ShellDataConnectionObject* connectionObject();
    [[nodiscard]] const ShellReadOnlyDataViewModel& summaryViewModel() const noexcept;
    [[nodiscard]] QVariantMap summaryViewModelMap() const;
    [[nodiscard]] ShellAccountListModel* accountModel();
    [[nodiscard]] ShellPortfolioListModel* portfolioModel();
    [[nodiscard]] ShellInstrumentListModel* instrumentModel();
    [[nodiscard]] ShellStrategyListModel* strategyModel();
    [[nodiscard]] const std::string& lastError() const noexcept;
    [[nodiscard]] QString lastErrorText() const;

signals:
    void summaryChanged();
    void lastErrorChanged();

private:
    [[nodiscard]] ShellDataResult<bool> failureFromResponse(
        const ShellDataResponse& response,
        const char* fallbackMessage);
    [[nodiscard]] ShellDataResult<bool> failureFromError(
        const ShellDataError& error,
        const char* fallbackMessage);
    void setLastError(std::string message);
    void clearLastError();
    void notifySummaryChanged();

    ShellReadOnlyDataFacade facade_;
    ShellDataConnectionObject connection_;
    ShellReadOnlyDataViewModel summary_;
    ShellAccountListModel accounts_;
    ShellPortfolioListModel portfolios_;
    ShellInstrumentListModel instruments_;
    ShellStrategyListModel strategies_;
    std::string lastError_;
};

}  // namespace etfdt::shell_services
