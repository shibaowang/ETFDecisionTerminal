#pragma once

#include "CoreDomain/Enums.h"
#include "CoreDomain/Result.h"

#include <string_view>

namespace etfdt::protocol {

using ErrorLevel = etfdt::core::LogLevel;

enum class ErrorCode {
    E1000_UNKNOWN_PROTOCOL_ERROR = 1000,
    E1001_INVALID_JSON = 1001,
    E1002_MISSING_REQUIRED_FIELD = 1002,
    E1003_UNSUPPORTED_PROTOCOL_VERSION = 1003,
    E1004_INVALID_ACTION = 1004,
    E1005_INVALID_SERVICE_NAME = 1005,

    E2000_DATABASE_ERROR = 2000,
    E2001_DATABASE_OPEN_FAILED = 2001,
    E2002_MIGRATION_FAILED = 2002,
    E2003_TRANSACTION_FAILED = 2003,

    E3000_ACCOUNTING_ERROR = 3000,
    E3001_REPLAY_FAILED = 3001,
    E3002_CASH_AUDIT_FAILED = 3002,
    E3003_SAFE_READONLY_REQUIRED = 3003,

    E4000_STRATEGY_ERROR = 4000,
    E4001_INVALID_STRATEGY_CONFIG = 4001,

    E5000_MARKET_ERROR = 5000,
    E5001_MARKET_DATA_STALE = 5001,
    E5002_MARKET_PROVIDER_FAILED = 5002,

    E6000_TRADE_DRAFT_ERROR = 6000,
    E6001_DRAFT_EXPIRED = 6001,
    E6002_DRAFT_SUPERSEDED = 6002,
    E6003_EXECUTION_CONFIRM_FAILED = 6003,

    E7000_IMPORT_ERROR = 7000,
    E7001_EXCEL_SCHEMA_INVALID = 7001,
    E7002_IMPORT_VALIDATION_FAILED = 7002,

    E8000_SECURITY_ERROR = 8000,
    E8001_AUTH_REQUIRED = 8001,
    E8002_PERMISSION_DENIED = 8002,

    E9000_SERVICE_ERROR = 9000,
    E9001_SERVICE_UNAVAILABLE = 9001,
    E9002_HEARTBEAT_TIMEOUT = 9002,
};

[[nodiscard]] std::string_view toString(ErrorCode value) noexcept;
[[nodiscard]] int numericCode(ErrorCode value) noexcept;
[[nodiscard]] std::string_view errorCategory(ErrorCode value) noexcept;
[[nodiscard]] ErrorLevel defaultSeverity(ErrorCode value) noexcept;
[[nodiscard]] etfdt::core::Result<ErrorCode> errorCodeFromString(std::string_view value);

template <typename T>
[[nodiscard]] etfdt::core::Result<T> fromString(std::string_view value);

template <>
[[nodiscard]] etfdt::core::Result<ErrorCode> fromString<ErrorCode>(std::string_view value);

}  // namespace etfdt::protocol
