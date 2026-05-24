#include "Protocol/ErrorCode.h"

#include <array>
#include <string>

namespace etfdt::protocol {
namespace {

constexpr std::array<std::pair<ErrorCode, std::string_view>, 32> kErrorCodes = {
    {{ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR, "E1000_UNKNOWN_PROTOCOL_ERROR"},
     {ErrorCode::E1001_INVALID_JSON, "E1001_INVALID_JSON"},
     {ErrorCode::E1002_MISSING_REQUIRED_FIELD, "E1002_MISSING_REQUIRED_FIELD"},
     {ErrorCode::E1003_UNSUPPORTED_PROTOCOL_VERSION, "E1003_UNSUPPORTED_PROTOCOL_VERSION"},
     {ErrorCode::E1004_INVALID_ACTION, "E1004_INVALID_ACTION"},
     {ErrorCode::E1005_INVALID_SERVICE_NAME, "E1005_INVALID_SERVICE_NAME"},
     {ErrorCode::E2000_DATABASE_ERROR, "E2000_DATABASE_ERROR"},
     {ErrorCode::E2001_DATABASE_OPEN_FAILED, "E2001_DATABASE_OPEN_FAILED"},
     {ErrorCode::E2002_MIGRATION_FAILED, "E2002_MIGRATION_FAILED"},
     {ErrorCode::E2003_TRANSACTION_FAILED, "E2003_TRANSACTION_FAILED"},
     {ErrorCode::E3000_ACCOUNTING_ERROR, "E3000_ACCOUNTING_ERROR"},
     {ErrorCode::E3001_REPLAY_FAILED, "E3001_REPLAY_FAILED"},
     {ErrorCode::E3002_CASH_AUDIT_FAILED, "E3002_CASH_AUDIT_FAILED"},
     {ErrorCode::E3003_SAFE_READONLY_REQUIRED, "E3003_SAFE_READONLY_REQUIRED"},
     {ErrorCode::E4000_STRATEGY_ERROR, "E4000_STRATEGY_ERROR"},
     {ErrorCode::E4001_INVALID_STRATEGY_CONFIG, "E4001_INVALID_STRATEGY_CONFIG"},
     {ErrorCode::E5000_MARKET_ERROR, "E5000_MARKET_ERROR"},
     {ErrorCode::E5001_MARKET_DATA_STALE, "E5001_MARKET_DATA_STALE"},
     {ErrorCode::E5002_MARKET_PROVIDER_FAILED, "E5002_MARKET_PROVIDER_FAILED"},
     {ErrorCode::E6000_TRADE_DRAFT_ERROR, "E6000_TRADE_DRAFT_ERROR"},
     {ErrorCode::E6001_DRAFT_EXPIRED, "E6001_DRAFT_EXPIRED"},
     {ErrorCode::E6002_DRAFT_SUPERSEDED, "E6002_DRAFT_SUPERSEDED"},
     {ErrorCode::E6003_EXECUTION_CONFIRM_FAILED, "E6003_EXECUTION_CONFIRM_FAILED"},
     {ErrorCode::E7000_IMPORT_ERROR, "E7000_IMPORT_ERROR"},
     {ErrorCode::E7001_EXCEL_SCHEMA_INVALID, "E7001_EXCEL_SCHEMA_INVALID"},
     {ErrorCode::E7002_IMPORT_VALIDATION_FAILED, "E7002_IMPORT_VALIDATION_FAILED"},
     {ErrorCode::E8000_SECURITY_ERROR, "E8000_SECURITY_ERROR"},
     {ErrorCode::E8001_AUTH_REQUIRED, "E8001_AUTH_REQUIRED"},
     {ErrorCode::E8002_PERMISSION_DENIED, "E8002_PERMISSION_DENIED"},
     {ErrorCode::E9000_SERVICE_ERROR, "E9000_SERVICE_ERROR"},
     {ErrorCode::E9001_SERVICE_UNAVAILABLE, "E9001_SERVICE_UNAVAILABLE"},
     {ErrorCode::E9002_HEARTBEAT_TIMEOUT, "E9002_HEARTBEAT_TIMEOUT"}}};

}  // namespace

std::string_view toString(ErrorCode value) noexcept
{
    for (const auto& entry : kErrorCodes) {
        if (entry.first == value) {
            return entry.second;
        }
    }
    return {};
}

int numericCode(ErrorCode value) noexcept
{
    return static_cast<int>(value);
}

std::string_view errorCategory(ErrorCode value) noexcept
{
    const int code = numericCode(value);
    if (code >= 1000 && code <= 1999) {
        return "PROTOCOL";
    }
    if (code >= 2000 && code <= 2999) {
        return "DATABASE";
    }
    if (code >= 3000 && code <= 3999) {
        return "ACCOUNTING";
    }
    if (code >= 4000 && code <= 4999) {
        return "STRATEGY";
    }
    if (code >= 5000 && code <= 5999) {
        return "MARKET";
    }
    if (code >= 6000 && code <= 6999) {
        return "TRADE";
    }
    if (code >= 7000 && code <= 7999) {
        return "IMPORT";
    }
    if (code >= 8000 && code <= 8999) {
        return "SECURITY";
    }
    if (code >= 9000 && code <= 9999) {
        return "SERVICE";
    }
    return "UNKNOWN";
}

ErrorLevel defaultSeverity(ErrorCode value) noexcept
{
    switch (value) {
    case ErrorCode::E3002_CASH_AUDIT_FAILED:
    case ErrorCode::E3003_SAFE_READONLY_REQUIRED:
    case ErrorCode::E6003_EXECUTION_CONFIRM_FAILED:
    case ErrorCode::E9002_HEARTBEAT_TIMEOUT:
        return ErrorLevel::Critical;
    case ErrorCode::E5001_MARKET_DATA_STALE:
    case ErrorCode::E6001_DRAFT_EXPIRED:
    case ErrorCode::E6002_DRAFT_SUPERSEDED:
        return ErrorLevel::Warning;
    default:
        return ErrorLevel::Error;
    }
}

etfdt::core::Result<ErrorCode> errorCodeFromString(std::string_view value)
{
    for (const auto& entry : kErrorCodes) {
        if (entry.second == value) {
            return etfdt::core::Result<ErrorCode>::success(entry.first);
        }
    }
    return etfdt::core::Result<ErrorCode>::failure(
        "Unknown ErrorCode: " + std::string(value));
}

template <>
etfdt::core::Result<ErrorCode> fromString<ErrorCode>(std::string_view value)
{
    return errorCodeFromString(value);
}

}  // namespace etfdt::protocol
