#include "DataAccess/ShellAccountingAuditWriteRepository.h"

#include "DataAccess/AuditLogRepository.h"
#include "DataAccess/TransactionRunner.h"

#include <sstream>

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

std::string jsonString(std::string value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (const char ch : value) {
        switch (ch) {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped.push_back(ch);
            break;
        }
    }
    return "\"" + escaped + "\"";
}

std::string auditReason(const ShellAccountingAuditWriteRequest& request)
{
    return "ShellAccounting snapshot write audit: " + request.snapshotBatchId;
}

std::string sanitizedNewValue(const ShellAccountingAuditWriteRequest& request)
{
    std::ostringstream stream;
    stream << "{"
           << "\"sourceAction\":" << jsonString(request.sourceAction) << ','
           << "\"authorizationToken\":" << jsonString(request.authorizationToken) << ','
           << "\"snapshotBatchId\":" << jsonString(request.snapshotBatchId) << ','
           << "\"resultStatus\":" << jsonString(request.sourceStatus) << ','
           << "\"sanitizedIssueCode\":" << jsonString(request.sanitizedIssueCode) << ','
           << "\"rowsWritten\":{"
           << "\"cash_snapshot\":" << request.cashRows << ','
           << "\"position_snapshot\":" << request.positionRows << ','
           << "\"portfolio_summary\":" << request.portfolioSummaryRows
           << "},"
           << "\"privacy\":{"
           << "\"rawSqlExposed\":false,"
           << "\"rawTradeLogPayloadExposed\":false,"
           << "\"fullSnapshotPayloadExposed\":false,"
           << "\"internalStackExposed\":false"
           << "}"
           << "}";
    return stream.str();
}

DatabaseResult<bool> validateRequest(const ShellAccountingAuditWriteRequest& request)
{
    if (request.accountId <= 0 || request.portfolioId <= 0) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting audit write requires accountId and portfolioId");
    }
    if (isBlank(request.snapshotBatchId)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting audit write requires snapshotBatchId");
    }
    if (request.sourceAction != "accounting.snapshot.write") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting audit write requires accounting.snapshot.write source action");
    }
    if (request.sourceStatus != "OK") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting audit write requires successful snapshot write result");
    }
    if (request.authorizationToken != "TASK-146_AUDIT_WRITE") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "ShellAccounting audit write requires TASK-146_AUDIT_WRITE authorization");
    }
    return DatabaseResult<bool>::success(true);
}

}  // namespace

ShellAccountingAuditWriteRepository::ShellAccountingAuditWriteRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<ShellAccountingAuditWriteResult>
ShellAccountingAuditWriteRepository::writeSnapshotWriteAudit(
    const ShellAccountingAuditWriteRequest& request)
{
    auto validation = validateRequest(request);
    if (!validation) {
        return DatabaseResult<ShellAccountingAuditWriteResult>::failure(
            validation.error().errorCode,
            validation.error().message);
    }

    auto existing = findExistingAuditEvent(request);
    if (!existing) {
        return DatabaseResult<ShellAccountingAuditWriteResult>::failure(
            existing.error().errorCode,
            existing.error().message);
    }
    if (existing.value().has_value()) {
        ShellAccountingAuditWriteResult result;
        result.auditLogId = *existing.value();
        result.duplicateEvent = true;
        return DatabaseResult<ShellAccountingAuditWriteResult>::success(result);
    }

    ShellAccountingAuditWriteResult result;
    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto inserted = insertAuditEvent(request);
        if (!inserted) {
            return DatabaseResult<bool>::failure(
                inserted.error().errorCode,
                inserted.error().message);
        }
        result.auditLogId = inserted.value();
        result.auditWritten = true;
        return DatabaseResult<bool>::success(true);
    });

    if (!transactionResult) {
        return DatabaseResult<ShellAccountingAuditWriteResult>::failure(
            transactionResult.error().errorCode,
            transactionResult.error().message);
    }

    result.transactionCommitted = true;
    return DatabaseResult<ShellAccountingAuditWriteResult>::success(result);
}

DatabaseResult<std::optional<std::int64_t>>
ShellAccountingAuditWriteRepository::findExistingAuditEvent(
    const ShellAccountingAuditWriteRequest& request)
{
    auto rows = connection_.queryRows(
        "SELECT id FROM audit_log "
        "WHERE account_id = ? AND portfolio_id = ? AND entity_type = ? AND action = ? AND reason = ? "
        "ORDER BY id DESC LIMIT 1;",
        {std::to_string(request.accountId),
         std::to_string(request.portfolioId),
         "SHELL_ACCOUNTING_SNAPSHOT_WRITE",
         "ACCOUNTING_SNAPSHOT_WRITE_AUDIT",
         auditReason(request)});
    if (!rows) {
        return DatabaseResult<std::optional<std::int64_t>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty()) {
        return DatabaseResult<std::optional<std::int64_t>>::success(std::nullopt);
    }
    return DatabaseResult<std::optional<std::int64_t>>::success(rows.value().front().front().int64Value);
}

DatabaseResult<std::int64_t> ShellAccountingAuditWriteRepository::insertAuditEvent(
    const ShellAccountingAuditWriteRequest& request)
{
    AuditLogEntry entry;
    entry.accountId = request.accountId;
    entry.portfolioId = request.portfolioId;
    entry.entityType = "SHELL_ACCOUNTING_SNAPSHOT_WRITE";
    entry.entityId = request.accountId;
    entry.action = "ACCOUNTING_SNAPSHOT_WRITE_AUDIT";
    entry.oldValueJson = "{}";
    entry.newValueJson = sanitizedNewValue(request);
    entry.reason = auditReason(request);
    entry.operatorName = "dataservice";
    entry.createdAtUtc = request.createdAtUtc;

    AuditLogRepository auditLogs(connection_);
    return auditLogs.insertAuditLog(entry);
}

}  // namespace etfdt::data_access
