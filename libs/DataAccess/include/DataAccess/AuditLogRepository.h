#pragma once

#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace etfdt::data_access {

struct AuditLogEntry final {
    std::optional<std::int64_t> accountId;
    std::optional<std::int64_t> portfolioId;
    std::string entityType;
    std::optional<std::int64_t> entityId;
    std::string action;
    std::string oldValueJson = "{}";
    std::string newValueJson = "{}";
    std::string reason;
    std::string operatorName = "local-user";
    std::string createdAtUtc;
};

struct AuditLogRecord final {
    std::int64_t id = 0;
    std::string uid;
    std::optional<std::int64_t> accountId;
    std::optional<std::int64_t> portfolioId;
    std::string entityType;
    std::optional<std::int64_t> entityId;
    std::string action;
    std::string oldValueJson;
    std::string newValueJson;
    std::string reason;
    std::string operatorName;
    std::string createdAtUtc;
};

class AuditLogRepository final {
public:
    explicit AuditLogRepository(SQLiteConnection& connection) noexcept;

    AuditLogRepository(const AuditLogRepository&) = delete;
    AuditLogRepository& operator=(const AuditLogRepository&) = delete;

    [[nodiscard]] DatabaseResult<std::int64_t> insertAuditLog(const AuditLogEntry& entry);
    [[nodiscard]] DatabaseResult<std::vector<AuditLogRecord>> listRecentAuditLogs(int limit) const;

private:
    SQLiteConnection& connection_;
};

}  // namespace etfdt::data_access
