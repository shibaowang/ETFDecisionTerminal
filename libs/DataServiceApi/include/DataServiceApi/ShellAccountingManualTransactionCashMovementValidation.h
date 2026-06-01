#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::data_service_api {

enum class ShellAccountingManualTradeSide {
    Unknown = 0,
    Buy,
    Sell,
};

enum class ShellAccountingManualCashMovementType {
    Unknown = 0,
    Deposit,
    Withdrawal,
};

struct ShellAccountingManualEntryValidationIssue {
    std::string code;
    std::string message;
    std::string field;
};

struct ShellAccountingManualEntryValidationResult {
    bool accepted = false;
    std::vector<ShellAccountingManualEntryValidationIssue> issues;

    [[nodiscard]] bool valid() const noexcept { return accepted && issues.empty(); }
};

struct ShellAccountingManualTransactionEntry {
    std::string accountId;
    std::string portfolioId;
    std::string securityCode;
    ShellAccountingManualTradeSide tradeSide = ShellAccountingManualTradeSide::Unknown;
    std::int64_t quantityUnits = 0;
    std::int64_t priceAmountMinor = 0;
    std::int64_t grossAmountMinor = 0;
    std::int64_t feeAmountMinor = 0;
    std::int64_t taxAmountMinor = 0;
    std::string occurredAt;
    std::string sourceMemo;
};

struct ShellAccountingManualCashMovementEntry {
    std::string accountId;
    std::string portfolioId;
    ShellAccountingManualCashMovementType movementType = ShellAccountingManualCashMovementType::Unknown;
    std::int64_t amountMinor = 0;
    std::string occurredAt;
    std::string sourceMemo;
};

[[nodiscard]] ShellAccountingManualEntryValidationResult validateManualTransactionEntry(
    const ShellAccountingManualTransactionEntry& entry);

[[nodiscard]] ShellAccountingManualEntryValidationResult validateManualCashMovement(
    const ShellAccountingManualCashMovementEntry& entry);

[[nodiscard]] bool containsSensitiveManualEntryMemoToken(const std::string& memo);

}  // namespace etfdt::data_service_api
