#include "DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <utility>

namespace etfdt::data_service_api {
namespace {

[[nodiscard]] bool blank(const std::string& value)
{
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
}

[[nodiscard]] std::string lowerCopy(const std::string& value)
{
    std::string lowered = value;
    std::transform(lowered.begin(), lowered.end(), lowered.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return lowered;
}

void addIssue(
    ShellAccountingManualEntryValidationResult& result,
    std::string code,
    std::string field,
    std::string message)
{
    result.issues.push_back(ShellAccountingManualEntryValidationIssue{
        std::move(code),
        std::move(message),
        std::move(field),
    });
}

void validateCommonIds(
    ShellAccountingManualEntryValidationResult& result,
    const std::string& accountId,
    const std::string& portfolioId)
{
    if (blank(accountId)) {
        addIssue(result, "ACCOUNT_ID_REQUIRED", "accountId", "Account id is required.");
    }
    if (blank(portfolioId)) {
        addIssue(result, "PORTFOLIO_ID_REQUIRED", "portfolioId", "Portfolio id is required.");
    }
}

void validateMemo(
    ShellAccountingManualEntryValidationResult& result,
    const std::string& memo)
{
    if (containsSensitiveManualEntryMemoToken(memo)) {
        addIssue(result, "SENSITIVE_MEMO_TOKEN", "sourceMemo", "Memo contains a sensitive token.");
    }
}

void finalize(ShellAccountingManualEntryValidationResult& result)
{
    result.accepted = result.issues.empty();
}

}  // namespace

bool containsSensitiveManualEntryMemoToken(const std::string& memo)
{
    const auto lowered = lowerCopy(memo);
    const char* sensitiveTokens[] = {
        "credential",
        "password",
        "secret",
        "token",
        "api_key",
        "apikey",
        "access_key",
        "private_key",
        "bearer ",
        "endpoint=",
    };

    return std::any_of(std::begin(sensitiveTokens), std::end(sensitiveTokens), [&](const char* token) {
        return lowered.find(token) != std::string::npos;
    });
}

ShellAccountingManualEntryValidationResult validateManualTransactionEntry(
    const ShellAccountingManualTransactionEntry& entry)
{
    ShellAccountingManualEntryValidationResult result;

    validateCommonIds(result, entry.accountId, entry.portfolioId);
    if (blank(entry.securityCode)) {
        addIssue(result, "SECURITY_CODE_REQUIRED", "securityCode", "Security code is required.");
    }
    if (entry.tradeSide != ShellAccountingManualTradeSide::Buy &&
        entry.tradeSide != ShellAccountingManualTradeSide::Sell) {
        addIssue(result, "TRADE_SIDE_INVALID", "tradeSide", "Trade side must be Buy or Sell.");
    }
    if (entry.quantityUnits <= 0) {
        addIssue(result, "QUANTITY_POSITIVE_REQUIRED", "quantityUnits", "Quantity must be positive.");
    }
    if (entry.priceAmountMinor <= 0) {
        addIssue(result, "PRICE_POSITIVE_REQUIRED", "priceAmountMinor", "Price must be positive.");
    }
    if (entry.grossAmountMinor <= 0) {
        addIssue(result, "GROSS_AMOUNT_POSITIVE_REQUIRED", "grossAmountMinor", "Gross amount must be positive.");
    }
    if (entry.feeAmountMinor < 0) {
        addIssue(result, "FEE_NON_NEGATIVE_REQUIRED", "feeAmountMinor", "Fee amount must not be negative.");
    }
    if (entry.taxAmountMinor < 0) {
        addIssue(result, "TAX_NON_NEGATIVE_REQUIRED", "taxAmountMinor", "Tax amount must not be negative.");
    }
    if (blank(entry.occurredAt)) {
        addIssue(result, "OCCURRED_AT_REQUIRED", "occurredAt", "Occurred time is required.");
    }
    validateMemo(result, entry.sourceMemo);

    finalize(result);
    return result;
}

ShellAccountingManualEntryValidationResult validateManualCashMovement(
    const ShellAccountingManualCashMovementEntry& entry)
{
    ShellAccountingManualEntryValidationResult result;

    validateCommonIds(result, entry.accountId, entry.portfolioId);
    if (entry.movementType != ShellAccountingManualCashMovementType::Deposit &&
        entry.movementType != ShellAccountingManualCashMovementType::Withdrawal) {
        addIssue(result, "CASH_MOVEMENT_TYPE_INVALID", "movementType", "Cash movement type must be Deposit or Withdrawal.");
    }
    if (entry.amountMinor <= 0) {
        addIssue(result, "CASH_AMOUNT_POSITIVE_REQUIRED", "amountMinor", "Cash amount must be positive.");
    }
    if (blank(entry.occurredAt)) {
        addIssue(result, "OCCURRED_AT_REQUIRED", "occurredAt", "Occurred time is required.");
    }
    validateMemo(result, entry.sourceMemo);

    finalize(result);
    return result;
}

}  // namespace etfdt::data_service_api
