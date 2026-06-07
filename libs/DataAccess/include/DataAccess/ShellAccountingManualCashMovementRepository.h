#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::data_access {

struct ShellAccountingManualCashMovementWriteCommand final {
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::string movementType;
    std::int64_t amountMinor = 0;
    std::string currencyCode;
    std::string occurredAtUtc;
    std::string sourceMemo;
    std::string sourceReference;
    std::string requestId;
    std::string idempotencyKey;
};

struct ShellAccountingManualCashMovementWriteResult final {
    bool accepted = false;
    bool success = false;
    bool writeImplemented = true;
    bool databaseWritten = false;
    bool tradeLogWritten = false;
    bool cashAdjustmentWritten = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool idempotentReplay = false;
    bool duplicate = false;
    bool transactionCommitted = false;
    std::int64_t tradeLogId = 0;
    std::int64_t cashAdjustmentId = 0;
    std::string tradeLogUid;
    std::string cashAdjustmentUid;
    std::string requestId;
    std::string idempotencyKey;
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
    std::string status = "MANUAL_CASH_MOVEMENT_REPOSITORY_NOT_RUN";
    std::vector<std::string> issues;
};

class ShellAccountingManualCashMovementRepository final {
public:
    explicit ShellAccountingManualCashMovementRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] ShellAccountingManualCashMovementWriteResult persistManualCashMovement(
        const ShellAccountingManualCashMovementWriteCommand& command);
    [[nodiscard]] ShellAccountingManualCashMovementWriteResult persistManualCashMovementInActiveTransaction(
        const ShellAccountingManualCashMovementWriteCommand& command);

private:
    [[nodiscard]] ShellAccountingManualCashMovementWriteResult failure(
        etfdt::protocol::ErrorCode code,
        std::string status,
        std::string issue) const;
    [[nodiscard]] ShellAccountingManualCashMovementWriteResult validate(
        const ShellAccountingManualCashMovementWriteCommand& command) const;
    [[nodiscard]] DatabaseResult<ShellAccountingManualCashMovementWriteResult> findExisting(
        const ShellAccountingManualCashMovementWriteCommand& command);
    [[nodiscard]] DatabaseResult<bool> requireReferencedRows(
        const ShellAccountingManualCashMovementWriteCommand& command);
    [[nodiscard]] DatabaseResult<std::int64_t> insertTradeLog(
        const ShellAccountingManualCashMovementWriteCommand& command,
        const std::string& tradeLogUid,
        const std::string& cashAdjustmentUid);
    [[nodiscard]] DatabaseResult<std::int64_t> insertCashAdjustment(
        const ShellAccountingManualCashMovementWriteCommand& command,
        const std::string& cashAdjustmentUid,
        const std::string& tradeLogUid,
        std::int64_t tradeLogId);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
