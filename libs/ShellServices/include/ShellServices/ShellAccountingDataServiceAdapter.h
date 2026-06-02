#pragma once

#include "ShellServices/ShellAccountingDataServiceClientPort.h"
#include "ShellServices/ShellAccountingServiceAdapter.h"

#include <memory>

namespace etfdt::shell_services {

class ShellAccountingDataServiceAdapter final : public ShellAccountingServiceAdapter {
public:
    ShellAccountingDataServiceAdapter() = default;
    explicit ShellAccountingDataServiceAdapter(
        std::shared_ptr<ShellAccountingDataServiceClientPort> clientPort);

    [[nodiscard]] ShellAccountingServiceResult fetchPositionList(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult fetchCashSummary(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult fetchPortfolioPnlSummary(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult fetchBasePositionSummary(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult fetchSniperPoolSummary(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult createDraft(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult confirmDraft(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult submitManualTransaction(
        const ShellAccountingServiceRequest& request) override;
    [[nodiscard]] ShellAccountingServiceResult submitManualCashMovement(
        const ShellAccountingServiceRequest& request) override;

    [[nodiscard]] bool hasLiveClient() const noexcept;
    [[nodiscard]] bool hasClientPort() const noexcept;
    void setClientPort(std::shared_ptr<ShellAccountingDataServiceClientPort> clientPort) noexcept;
    void clearClientPort() noexcept;
    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;

private:
    std::shared_ptr<ShellAccountingDataServiceClientPort> clientPort_;
};

}  // namespace etfdt::shell_services
