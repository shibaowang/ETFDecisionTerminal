#include "DataAccess/ShellAccountingSnapshotWriteRepository.h"

#include "DataAccess/TransactionRunner.h"

namespace etfdt::data_access {
namespace {

SqlStatementParameter optionalInt64(std::optional<std::int64_t> value)
{
    return value.has_value() ? SqlStatementParameter::int64ValueOf(*value)
                             : SqlStatementParameter::nullValue();
}

DatabaseResult<bool> propagateFailure(const DatabaseResult<bool>& result)
{
    return DatabaseResult<bool>::failure(result.error().errorCode, result.error().message);
}

}  // namespace

ShellAccountingSnapshotWriteRepository::ShellAccountingSnapshotWriteRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<ShellAccountingSnapshotWriteResult>
ShellAccountingSnapshotWriteRepository::writeSnapshotBatch(
    const ShellAccountingSnapshotWriteBatch& batch)
{
    ShellAccountingSnapshotWriteResult writeResult;
    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        for (const auto& row : batch.positions) {
            auto inserted = upsertPositionSnapshot(row);
            if (!inserted) {
                return propagateFailure(inserted);
            }
            ++writeResult.positionRows;
        }
        for (const auto& row : batch.cash) {
            auto inserted = upsertCashSnapshot(row);
            if (!inserted) {
                return propagateFailure(inserted);
            }
            ++writeResult.cashRows;
        }
        for (const auto& row : batch.portfolioSummaries) {
            auto inserted = upsertPortfolioSummary(row);
            if (!inserted) {
                return propagateFailure(inserted);
            }
            ++writeResult.portfolioSummaryRows;
        }
        return DatabaseResult<bool>::success(true);
    });

    if (!transactionResult) {
        return DatabaseResult<ShellAccountingSnapshotWriteResult>::failure(
            transactionResult.error().errorCode,
            transactionResult.error().message);
    }

    writeResult.transactionCommitted = true;
    return DatabaseResult<ShellAccountingSnapshotWriteResult>::success(writeResult);
}

DatabaseResult<bool> ShellAccountingSnapshotWriteRepository::upsertPositionSnapshot(
    const ShellAccountingPositionSnapshotWriteRow& row)
{
    return connection_.executeStatement(
        "INSERT INTO position_snapshot("
        "uid,account_id,portfolio_id,strategy_id,strategy_code,actual_instrument_id,"
        "actual_code,trade_source,quantity_1e6,cost_amount_cents,market_value_cents,"
        "unrealized_pnl_cents,updated_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?) "
        "ON CONFLICT(uid) DO UPDATE SET "
        "account_id=excluded.account_id,"
        "portfolio_id=excluded.portfolio_id,"
        "strategy_id=excluded.strategy_id,"
        "strategy_code=excluded.strategy_code,"
        "actual_instrument_id=excluded.actual_instrument_id,"
        "actual_code=excluded.actual_code,"
        "trade_source=excluded.trade_source,"
        "quantity_1e6=excluded.quantity_1e6,"
        "cost_amount_cents=excluded.cost_amount_cents,"
        "market_value_cents=excluded.market_value_cents,"
        "unrealized_pnl_cents=excluded.unrealized_pnl_cents,"
        "updated_at_utc=excluded.updated_at_utc",
        {
            SqlStatementParameter::textValue(row.uid),
            SqlStatementParameter::int64ValueOf(row.accountId),
            SqlStatementParameter::int64ValueOf(row.portfolioId),
            optionalInt64(row.strategyId),
            row.strategyCode.empty() ? SqlStatementParameter::nullValue()
                                     : SqlStatementParameter::textValue(row.strategyCode),
            optionalInt64(row.actualInstrumentId),
            row.actualCode.empty() ? SqlStatementParameter::nullValue()
                                   : SqlStatementParameter::textValue(row.actualCode),
            SqlStatementParameter::textValue(row.tradeSource),
            SqlStatementParameter::int64ValueOf(row.quantity1e6),
            SqlStatementParameter::int64ValueOf(row.costAmountCents),
            SqlStatementParameter::int64ValueOf(row.marketValueCents),
            SqlStatementParameter::int64ValueOf(row.unrealizedPnlCents),
            SqlStatementParameter::textValue(row.updatedAtUtc),
        });
}

DatabaseResult<bool> ShellAccountingSnapshotWriteRepository::upsertCashSnapshot(
    const ShellAccountingCashSnapshotWriteRow& row)
{
    return connection_.executeStatement(
        "INSERT INTO cash_snapshot("
        "uid,account_id,portfolio_id,cash_balance_cents,principal_base_cents,updated_at_utc) "
        "VALUES(?,?,?,?,?,?) "
        "ON CONFLICT(uid) DO UPDATE SET "
        "account_id=excluded.account_id,"
        "portfolio_id=excluded.portfolio_id,"
        "cash_balance_cents=excluded.cash_balance_cents,"
        "principal_base_cents=excluded.principal_base_cents,"
        "updated_at_utc=excluded.updated_at_utc",
        {
            SqlStatementParameter::textValue(row.uid),
            SqlStatementParameter::int64ValueOf(row.accountId),
            SqlStatementParameter::int64ValueOf(row.portfolioId),
            SqlStatementParameter::int64ValueOf(row.cashBalanceCents),
            SqlStatementParameter::int64ValueOf(row.principalBaseCents),
            SqlStatementParameter::textValue(row.updatedAtUtc),
        });
}

DatabaseResult<bool> ShellAccountingSnapshotWriteRepository::upsertPortfolioSummary(
    const ShellAccountingPortfolioSummaryWriteRow& row)
{
    return connection_.executeStatement(
        "INSERT INTO portfolio_summary("
        "uid,account_id,portfolio_id,total_assets_cents,total_market_value_cents,"
        "cash_balance_cents,principal_base_cents,holding_cost_cents,total_pnl_cents,"
        "total_return_ppm,base_completion_ratio_ppm,sniper_pool_cents,"
        "active_grid_cycle_count,active_draft_count,base_violation_count,updated_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) "
        "ON CONFLICT(uid) DO UPDATE SET "
        "account_id=excluded.account_id,"
        "portfolio_id=excluded.portfolio_id,"
        "total_assets_cents=excluded.total_assets_cents,"
        "total_market_value_cents=excluded.total_market_value_cents,"
        "cash_balance_cents=excluded.cash_balance_cents,"
        "principal_base_cents=excluded.principal_base_cents,"
        "holding_cost_cents=excluded.holding_cost_cents,"
        "total_pnl_cents=excluded.total_pnl_cents,"
        "total_return_ppm=excluded.total_return_ppm,"
        "base_completion_ratio_ppm=excluded.base_completion_ratio_ppm,"
        "sniper_pool_cents=excluded.sniper_pool_cents,"
        "active_grid_cycle_count=excluded.active_grid_cycle_count,"
        "active_draft_count=excluded.active_draft_count,"
        "base_violation_count=excluded.base_violation_count,"
        "updated_at_utc=excluded.updated_at_utc",
        {
            SqlStatementParameter::textValue(row.uid),
            SqlStatementParameter::int64ValueOf(row.accountId),
            SqlStatementParameter::int64ValueOf(row.portfolioId),
            SqlStatementParameter::int64ValueOf(row.totalAssetsCents),
            SqlStatementParameter::int64ValueOf(row.totalMarketValueCents),
            SqlStatementParameter::int64ValueOf(row.cashBalanceCents),
            SqlStatementParameter::int64ValueOf(row.principalBaseCents),
            SqlStatementParameter::int64ValueOf(row.holdingCostCents),
            SqlStatementParameter::int64ValueOf(row.totalPnlCents),
            SqlStatementParameter::int64ValueOf(row.totalReturnPpm),
            SqlStatementParameter::int64ValueOf(row.baseCompletionRatioPpm),
            SqlStatementParameter::int64ValueOf(row.sniperPoolCents),
            SqlStatementParameter::int64ValueOf(row.activeGridCycleCount),
            SqlStatementParameter::int64ValueOf(row.activeDraftCount),
            SqlStatementParameter::int64ValueOf(row.baseViolationCount),
            SqlStatementParameter::textValue(row.updatedAtUtc),
        });
}

}  // namespace etfdt::data_access
