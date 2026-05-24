#include "DataAccess/AuditLogRepository.h"

#include "Protocol/Json.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

namespace etfdt::data_access {
namespace {

bool isBlank(const std::string& value)
{
    for (const char ch : value) {
        if (ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
            return false;
        }
    }
    return true;
}

std::string currentUtcTimestamp()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm utc{};
    gmtime_s(&utc, &time);

    std::ostringstream stream;
    stream << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return stream.str();
}

std::string makeAuditUid()
{
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    std::ostringstream stream;
    stream << "audit-" << ticks;
    return stream.str();
}

SqlStatementParameter optionalInt64Parameter(const std::optional<std::int64_t>& value)
{
    if (!value.has_value()) {
        return SqlStatementParameter::nullValue();
    }
    return SqlStatementParameter::int64ValueOf(*value);
}

DatabaseResult<bool> validateAuditLogEntry(const AuditLogEntry& entry)
{
    if (!entry.accountId.has_value() && entry.portfolioId.has_value()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "audit_log portfolioId requires accountId");
    }
    if (isBlank(entry.entityType)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "audit_log entityType is required");
    }
    if (isBlank(entry.action)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "audit_log action is required");
    }
    if (isBlank(entry.reason)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "audit_log reason is required");
    }
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(entry.oldValueJson)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "audit_log oldValueJson must be a JSON object or array fragment");
    }
    if (!etfdt::protocol::isLikelyJsonObjectOrArray(entry.newValueJson)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "audit_log newValueJson must be a JSON object or array fragment");
    }
    return DatabaseResult<bool>::success(true);
}

std::optional<std::int64_t> nullableInt64(const SqlValue& value)
{
    if (value.isNull) {
        return std::nullopt;
    }
    return value.int64Value;
}

DatabaseResult<AuditLogRecord> parseAuditLogRecord(const SqlQueryRow& row)
{
    if (row.size() != 12U) {
        return DatabaseResult<AuditLogRecord>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "audit_log query returned unexpected column count");
    }

    AuditLogRecord record;
    record.id = row[0].int64Value;
    record.uid = row[1].text;
    record.accountId = nullableInt64(row[2]);
    record.portfolioId = nullableInt64(row[3]);
    record.entityType = row[4].text;
    record.entityId = nullableInt64(row[5]);
    record.action = row[6].text;
    record.oldValueJson = row[7].text;
    record.newValueJson = row[8].text;
    record.reason = row[9].text;
    record.operatorName = row[10].text;
    record.createdAtUtc = row[11].text;
    return DatabaseResult<AuditLogRecord>::success(std::move(record));
}

}  // namespace

AuditLogRepository::AuditLogRepository(SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<std::int64_t> AuditLogRepository::insertAuditLog(const AuditLogEntry& entry)
{
    if (!connection_.isInTransaction()) {
        return DatabaseResult<std::int64_t>::failure(
            etfdt::protocol::ErrorCode::E2003_TRANSACTION_FAILED,
            "insertAuditLog requires an active transaction");
    }

    auto validation = validateAuditLogEntry(entry);
    if (!validation) {
        return DatabaseResult<std::int64_t>::failure(
            validation.error().errorCode,
            validation.error().message);
    }

    const std::string uid = makeAuditUid();
    const std::string operatorName = isBlank(entry.operatorName) ? "local-user" : entry.operatorName;
    const std::string createdAtUtc =
        isBlank(entry.createdAtUtc) ? currentUtcTimestamp() : entry.createdAtUtc;

    auto inserted = connection_.executeStatement(
        "INSERT INTO audit_log "
        "(uid, account_id, portfolio_id, entity_type, entity_id, action, old_value_json, "
        "new_value_json, reason, operator, created_at_utc) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);",
        {SqlStatementParameter::textValue(uid),
         optionalInt64Parameter(entry.accountId),
         optionalInt64Parameter(entry.portfolioId),
         SqlStatementParameter::textValue(entry.entityType),
         optionalInt64Parameter(entry.entityId),
         SqlStatementParameter::textValue(entry.action),
         SqlStatementParameter::textValue(entry.oldValueJson),
         SqlStatementParameter::textValue(entry.newValueJson),
         SqlStatementParameter::textValue(entry.reason),
         SqlStatementParameter::textValue(operatorName),
         SqlStatementParameter::textValue(createdAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(
            inserted.error().errorCode,
            inserted.error().message);
    }

    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::vector<AuditLogRecord>> AuditLogRepository::listRecentAuditLogs(int limit) const
{
    if (limit <= 0) {
        return DatabaseResult<std::vector<AuditLogRecord>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "audit_log limit must be positive");
    }

    auto rows = connection_.queryRows(
        "SELECT id, uid, account_id, portfolio_id, entity_type, entity_id, action, "
        "COALESCE(old_value_json, '{}'), COALESCE(new_value_json, '{}'), "
        "COALESCE(reason, ''), COALESCE(operator, ''), created_at_utc "
        "FROM audit_log ORDER BY id DESC LIMIT ?;",
        {std::to_string(limit)});
    if (!rows) {
        return DatabaseResult<std::vector<AuditLogRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }

    std::vector<AuditLogRecord> records;
    records.reserve(rows.value().size());
    for (const auto& row : rows.value()) {
        auto record = parseAuditLogRecord(row);
        if (!record) {
            return DatabaseResult<std::vector<AuditLogRecord>>::failure(
                record.error().errorCode,
                record.error().message);
        }
        records.push_back(record.value());
    }
    return DatabaseResult<std::vector<AuditLogRecord>>::success(std::move(records));
}

}  // namespace etfdt::data_access
