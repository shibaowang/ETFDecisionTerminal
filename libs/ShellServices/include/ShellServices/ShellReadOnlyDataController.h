#pragma once

#include "ShellServices/ShellDataConnectionObject.h"
#include "ShellServices/ShellReadOnlyDataFacade.h"
#include "ShellServices/ShellReadOnlyDataViewModel.h"
#include "ShellServices/ShellReadOnlyListModels.h"

#include <QObject>
#include <string>

namespace etfdt::shell_services {

class ShellReadOnlyDataController final : public QObject {
    Q_OBJECT

public:
    explicit ShellReadOnlyDataController(QObject* parent = nullptr);

    [[nodiscard]] ShellDataResult<bool> connectToDataService(
        const std::string& socketName,
        int timeoutMs);
    void disconnect();

    [[nodiscard]] ShellDataResult<bool> refreshHealth(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<bool> refreshSummary(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<bool> refreshAccounts(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<bool> refreshPortfolios(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<bool> refreshInstruments(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<bool> refreshStrategies(int timeoutMs = 2000);
    [[nodiscard]] ShellDataResult<bool> refreshAll(int timeoutMs = 2000);

    [[nodiscard]] ShellDataConnectionObject* connectionObject();
    [[nodiscard]] const ShellReadOnlyDataViewModel& summaryViewModel() const noexcept;
    [[nodiscard]] ShellAccountListModel* accountModel();
    [[nodiscard]] ShellPortfolioListModel* portfolioModel();
    [[nodiscard]] ShellInstrumentListModel* instrumentModel();
    [[nodiscard]] ShellStrategyListModel* strategyModel();
    [[nodiscard]] const std::string& lastError() const noexcept;

private:
    [[nodiscard]] ShellDataResult<bool> failureFromResponse(
        const ShellDataResponse& response,
        const char* fallbackMessage);
    [[nodiscard]] ShellDataResult<bool> failureFromError(
        const ShellDataError& error,
        const char* fallbackMessage);
    void setLastError(std::string message);
    void clearLastError();

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
