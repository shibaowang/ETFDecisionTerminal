#pragma once

#include "DataAccess/ReadOnlyRepositoryBase.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace etfdt::data_access {

struct ShellAccountingFactsQueryRequest final {
    std::string accountId;
    std::string portfolioId;
};

struct ShellAccountingPositionRow final {
    std::string accountId;
    std::string portfolioId;
    std::string strategyCode;
    std::string instrumentCode;
    std::string tradeSource;
    std::string quantityText;
    std::string costAmountText;
    std::string marketValueText;
    std::string unrealizedPnlText;
    std::string updatedAtUtc;
};

struct ShellAccountingCashSummaryRow final {
    std::string accountId;
    std::string portfolioId;
    std::string cashBalanceText;
    std::string principalBaseText;
    std::string updatedAtUtc;
};

struct ShellAccountingPortfolioSummaryRow final {
    std::string accountId;
    std::string portfolioId;
    std::string totalAssetsText;
    std::string totalMarketValueText;
    std::string cashBalanceText;
    std::string principalBaseText;
    std::string holdingCostText;
    std::string totalPnlText;
    std::string totalReturnText;
    std::string baseCompletionRatioText;
    std::string sniperPoolText;
    int activeGridCycleCount = 0;
    int activeDraftCount = 0;
    int baseViolationCount = 0;
    std::string updatedAtUtc;
};

struct ShellAccountingBasePositionSummaryRow final {
    std::string accountId;
    std::string portfolioId;
    std::string strategyCode;
    std::string instrumentCode;
    std::string targetBaseAmountText;
    std::string allocatedAmountText;
    std::string updatedAtUtc;
};

struct ShellAccountingSniperPoolTierRow final {
    std::string accountId;
    std::string portfolioId;
    std::string strategyCode;
    int tierNo = 0;
    std::string tierName;
    std::string allocatedAmountText;
    std::string updatedAtUtc;
};

[[nodiscard]] std::string_view shellAccountingPositionListReadOnlySql() noexcept;
[[nodiscard]] std::string_view shellAccountingCashSummaryReadOnlySql() noexcept;
[[nodiscard]] std::string_view shellAccountingPortfolioSummaryReadOnlySql() noexcept;
[[nodiscard]] std::string_view shellAccountingBasePositionSummaryReadOnlySql() noexcept;
[[nodiscard]] std::string_view shellAccountingSniperPoolSummaryReadOnlySql() noexcept;

class ShellAccountingReadOnlyFactsQuery final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<std::vector<ShellAccountingPositionRow>> loadPositions(
        const ShellAccountingFactsQueryRequest& request) const;
    [[nodiscard]] RepositoryResult<std::vector<ShellAccountingCashSummaryRow>> loadCashSummaries(
        const ShellAccountingFactsQueryRequest& request) const;
    [[nodiscard]] RepositoryResult<std::vector<ShellAccountingPortfolioSummaryRow>>
    loadPortfolioSummaries(const ShellAccountingFactsQueryRequest& request) const;
    [[nodiscard]] RepositoryResult<std::vector<ShellAccountingBasePositionSummaryRow>>
    loadBasePositionSummaries(const ShellAccountingFactsQueryRequest& request) const;
    [[nodiscard]] RepositoryResult<std::vector<ShellAccountingSniperPoolTierRow>>
    loadSniperPoolTiers(const ShellAccountingFactsQueryRequest& request) const;
};

}  // namespace etfdt::data_access
