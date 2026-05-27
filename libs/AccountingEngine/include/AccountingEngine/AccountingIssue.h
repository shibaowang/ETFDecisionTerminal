#pragma once

#include <string>

namespace etfdt::accounting {

namespace AccountingIssueLevel {
inline constexpr const char* Info = "INFO";
inline constexpr const char* Warning = "WARNING";
inline constexpr const char* Error = "ERROR";
} // namespace AccountingIssueLevel

namespace AccountingIssueCode {
inline constexpr const char* MissingRequiredField = "MISSING_REQUIRED_FIELD";
inline constexpr const char* InvalidEnum = "INVALID_ENUM";
inline constexpr const char* InvalidTimeRange = "INVALID_TIME_RANGE";
inline constexpr const char* InvalidMoneyText = "INVALID_MONEY_TEXT";
inline constexpr const char* InvalidQuantityText = "INVALID_QUANTITY_TEXT";
inline constexpr const char* UnsupportedAction = "UNSUPPORTED_ACTION";
inline constexpr const char* UnsupportedCurrency = "UNSUPPORTED_CURRENCY";
inline constexpr const char* ReplayNotImplemented = "REPLAY_NOT_IMPLEMENTED";
inline constexpr const char* NegativeCash = "NEGATIVE_CASH";
inline constexpr const char* SellExceedsPosition = "SELL_EXCEEDS_POSITION";
inline constexpr const char* MissingFee = "MISSING_FEE";
} // namespace AccountingIssueCode

struct AccountingIssueDto {
    std::string level;
    std::string code;
    std::string message;
    bool blocking = false;
    std::string field;
    std::string sourceId;
};

[[nodiscard]] AccountingIssueDto makeAccountingIssue(
    std::string level,
    std::string code,
    std::string message,
    bool blocking,
    std::string field = {},
    std::string sourceId = {});

} // namespace etfdt::accounting
