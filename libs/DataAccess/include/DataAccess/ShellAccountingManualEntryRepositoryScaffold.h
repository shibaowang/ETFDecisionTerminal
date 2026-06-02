#pragma once

#include "Protocol/ErrorCode.h"

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::data_access {

struct ShellAccountingManualTransactionPersistenceCommand final {
    std::string accountId;
    std::string portfolioId;
    std::string securityCode;
    std::string tradeSide;
    std::int64_t quantityUnits = 0;
    std::int64_t priceAmountMinor = 0;
    std::int64_t grossAmountMinor = 0;
    std::int64_t feeAmountMinor = 0;
    std::int64_t taxAmountMinor = 0;
    std::string occurredAt;
    std::string sourceMemo;
    std::string requestId;
    std::string idempotencyKey;
};

struct ShellAccountingManualCashMovementPersistenceCommand final {
    std::string accountId;
    std::string portfolioId;
    std::string movementType;
    std::int64_t amountMinor = 0;
    std::string occurredAt;
    std::string sourceMemo;
    std::string requestId;
    std::string idempotencyKey;
};

struct ShellAccountingManualEntryPersistenceResult final {
    bool accepted = false;
    bool success = false;
    bool writeImplemented = false;
    bool databaseWritten = false;
    bool tradeLogWritten = false;
    bool cashFactsWritten = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    etfdt::protocol::ErrorCode errorCode = etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
    std::string status = "MANUAL_ENTRY_REPOSITORY_SCAFFOLD_DISABLED";
    std::vector<std::string> issues;
};

class ShellAccountingManualEntryRepositoryScaffold final {
public:
    [[nodiscard]] ShellAccountingManualEntryPersistenceResult persistManualTransaction(
        const ShellAccountingManualTransactionPersistenceCommand& command) const;

    [[nodiscard]] ShellAccountingManualEntryPersistenceResult persistManualCashMovement(
        const ShellAccountingManualCashMovementPersistenceCommand& command) const;
};

}  // namespace etfdt::data_access
