#pragma once

#include "ShellServices/ShellAccountingServiceAdapter.h"

namespace etfdt::shell_services {

class ShellAccountingDataServiceAdapter final : public ShellAccountingServiceAdapter {
public:
    ShellAccountingDataServiceAdapter() = default;

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

    [[nodiscard]] bool hasLiveClient() const noexcept;
    [[nodiscard]] bool readOnly() const noexcept;
    [[nodiscard]] bool writeEnabled() const noexcept;
};

}  // namespace etfdt::shell_services
