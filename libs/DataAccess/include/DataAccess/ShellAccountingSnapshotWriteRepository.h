#pragma once

#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace etfdt::data_access {

struct ShellAccountingPositionSnapshotWriteRow final {
    std::string uid;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::optional<std::int64_t> strategyId;
    std::string strategyCode;
    std::optional<std::int64_t> actualInstrumentId;
    std::string actualCode;
    std::string tradeSource = "SYSTEM";
    std::int64_t quantity1e6 = 0;
    std::int64_t costAmountCents = 0;
    std::int64_t marketValueCents = 0;
    std::int64_t unrealizedPnlCents = 0;
    std::string updatedAtUtc;
};

struct ShellAccountingCashSnapshotWriteRow final {
    std::string uid;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t cashBalanceCents = 0;
    std::int64_t principalBaseCents = 0;
    std::string updatedAtUtc;
};

struct ShellAccountingPortfolioSummaryWriteRow final {
    std::string uid;
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t totalAssetsCents = 0;
    std::int64_t totalMarketValueCents = 0;
    std::int64_t cashBalanceCents = 0;
    std::int64_t principalBaseCents = 0;
    std::int64_t holdingCostCents = 0;
    std::int64_t totalPnlCents = 0;
    std::int64_t totalReturnPpm = 0;
    std::int64_t baseCompletionRatioPpm = 0;
    std::int64_t sniperPoolCents = 0;
    std::int64_t activeGridCycleCount = 0;
    std::int64_t activeDraftCount = 0;
    std::int64_t baseViolationCount = 0;
    std::string updatedAtUtc;
};

struct ShellAccountingSnapshotWriteBatch final {
    std::vector<ShellAccountingPositionSnapshotWriteRow> positions;
    std::vector<ShellAccountingCashSnapshotWriteRow> cash;
    std::vector<ShellAccountingPortfolioSummaryWriteRow> portfolioSummaries;
};

struct ShellAccountingSnapshotWriteResult final {
    int positionRows = 0;
    int cashRows = 0;
    int portfolioSummaryRows = 0;
    bool transactionCommitted = false;
    bool idempotentUpsert = true;
};

class ShellAccountingSnapshotWriteRepository final {
public:
    explicit ShellAccountingSnapshotWriteRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] DatabaseResult<ShellAccountingSnapshotWriteResult> writeSnapshotBatch(
        const ShellAccountingSnapshotWriteBatch& batch);

private:
    [[nodiscard]] DatabaseResult<bool> upsertPositionSnapshot(
        const ShellAccountingPositionSnapshotWriteRow& row);
    [[nodiscard]] DatabaseResult<bool> upsertCashSnapshot(
        const ShellAccountingCashSnapshotWriteRow& row);
    [[nodiscard]] DatabaseResult<bool> upsertPortfolioSummary(
        const ShellAccountingPortfolioSummaryWriteRow& row);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
