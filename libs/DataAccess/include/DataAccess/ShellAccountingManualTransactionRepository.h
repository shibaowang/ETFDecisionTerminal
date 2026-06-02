#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::data_access {

struct ShellAccountingManualTransactionWriteCommand final {
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t instrumentId = 0;
    std::string securityCode;
    std::string tradeSide;
    std::int64_t quantityUnits = 0;
    std::int64_t priceAmountMinor = 0;
    std::int64_t grossAmountMinor = 0;
    std::int64_t feeAmountMinor = 0;
    std::int64_t taxAmountMinor = 0;
    std::string occurredAtUtc;
    std::string sourceMemo;
    std::string requestId;
    std::string idempotencyKey;
};

struct ShellAccountingManualTransactionWriteResult final {
    bool accepted = false;
    bool success = false;
    bool writeImplemented = true;
    bool databaseWritten = false;
    bool tradeExecutionGroupWritten = false;
    bool tradeLogWritten = false;
    bool cashFactsWritten = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool idempotentReplay = false;
    bool duplicate = false;
    bool transactionCommitted = false;
    std::int64_t tradeExecutionGroupId = 0;
    std::int64_t tradeLogId = 0;
    std::string tradeExecutionGroupUid;
    std::string tradeLogUid;
    std::string requestId;
    std::string idempotencyKey;
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
    std::string status = "MANUAL_TRANSACTION_REPOSITORY_NOT_RUN";
    std::vector<std::string> issues;
};

class ShellAccountingManualTransactionRepository final {
public:
    explicit ShellAccountingManualTransactionRepository(SQLiteConnection& connection) noexcept;

    [[nodiscard]] ShellAccountingManualTransactionWriteResult persistManualTransaction(
        const ShellAccountingManualTransactionWriteCommand& command);

private:
    [[nodiscard]] ShellAccountingManualTransactionWriteResult failure(
        etfdt::protocol::ErrorCode code,
        std::string status,
        std::string issue) const;
    [[nodiscard]] ShellAccountingManualTransactionWriteResult validate(
        const ShellAccountingManualTransactionWriteCommand& command) const;
    [[nodiscard]] DatabaseResult<ShellAccountingManualTransactionWriteResult> findExisting(
        const ShellAccountingManualTransactionWriteCommand& command);
    [[nodiscard]] DatabaseResult<bool> requireReferencedRows(
        const ShellAccountingManualTransactionWriteCommand& command);
    [[nodiscard]] DatabaseResult<std::int64_t> insertExecutionGroup(
        const ShellAccountingManualTransactionWriteCommand& command,
        const std::string& uid);
    [[nodiscard]] DatabaseResult<std::int64_t> insertTradeLog(
        const ShellAccountingManualTransactionWriteCommand& command,
        const std::string& uid,
        std::int64_t executionGroupId);

    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access

