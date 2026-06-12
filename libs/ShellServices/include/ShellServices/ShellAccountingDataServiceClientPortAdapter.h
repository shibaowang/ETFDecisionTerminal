#pragma once

#include "ShellServices/ShellAccountingDataServiceClientPort.h"

#include <memory>

namespace etfdt::data_service_client {
class DataServiceClient;
}  // namespace etfdt::data_service_client

namespace etfdt::shell_services {

class ShellAccountingDataServiceClientPortAdapter final
    : public ShellAccountingDataServiceClientPort {
public:
    ShellAccountingDataServiceClientPortAdapter() = default;
    explicit ShellAccountingDataServiceClientPortAdapter(
        std::shared_ptr<etfdt::data_service_client::DataServiceClient> client);

    [[nodiscard]] bool hasClient() const noexcept;
    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;

    [[nodiscard]] ShellAccountingDataServiceClientResponse callPositionList(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callCashSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callTradeDraftCreate(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callTradeDraftCreateFromRecommendation(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callTradeDraftReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callTradeDraftConfirm(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callManualTransactionCreate(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callManualCashMovementCreate(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callExcelVbaImportReadOnlyPreview(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callExcelVbaImportPersistManualEntry(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callPortfolioReplayReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callStrategyRecommendationReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callMarketDataRefreshReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callMarketDataHistoricalHighReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callOtcMapMultiChannelReadOnlyPreview(
        const ShellAccountingDataServiceClientRequest& request) override;
    [[nodiscard]] ShellAccountingDataServiceClientResponse callTradeDraftCreateOtcMapMultiChannel(
        const ShellAccountingDataServiceClientRequest& request) override;

private:
    std::shared_ptr<etfdt::data_service_client::DataServiceClient> client_;
};

}  // namespace etfdt::shell_services
