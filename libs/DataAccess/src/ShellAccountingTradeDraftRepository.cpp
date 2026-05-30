#include "DataAccess/ShellAccountingTradeDraftRepository.h"

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

std::string sanitizeUidPart(std::string value)
{
    for (char& ch : value) {
        const bool allowed = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_';
        if (!allowed) {
            ch = '-';
        }
    }
    return value;
}

std::string sourceId(const ShellAccountingTradeDraftCreateRequest& request)
{
    if (!isBlank(request.sourceSnapshotId)) {
        return request.sourceSnapshotId;
    }
    return request.sourceReplayId;
}

std::string draftSignature(const ShellAccountingTradeDraftCreateRequest& request)
{
    std::ostringstream stream;
    stream << "TASK-148|" << request.accountId << '|' << request.portfolioId << '|'
           << request.strategyCode << '|' << request.instrumentCode << '|' << request.side << '|'
           << request.quantity1e6 << '|' << sourceId(request);
    return stream.str();
}

std::string draftUid(const ShellAccountingTradeDraftCreateRequest& request)
{
    std::ostringstream stream;
    stream << "task-148-draft-" << request.accountId << '-' << request.portfolioId << '-'
           << sanitizeUidPart(request.strategyCode) << '-' << sanitizeUidPart(request.instrumentCode)
           << '-' << request.side << '-' << request.quantity1e6 << '-'
           << sanitizeUidPart(sourceId(request));
    return stream.str();
}

std::string auditReason(const ShellAccountingTradeDraftCreateRequest& request)
{
    return "ShellAccounting TradeDraft creation: " + draftSignature(request);
}

std::string sanitizedAuditPayload(
    const ShellAccountingTradeDraftCreateRequest& request,
    std::int64_t draftId,
    const std::string& uid)
{
    std::ostringstream stream;
    stream << "{"
           << "\"sourceAction\":\"accounting.tradedraft.create\","
           << "\"authorizationToken\":" << jsonString(request.authorizationToken) << ','
           << "\"draftId\":" << draftId << ','
           << "\"draftUid\":" << jsonString(uid) << ','
           << "\"draftSignature\":" << jsonString(draftSignature(request)) << ','
           << "\"accountId\":" << request.accountId << ','
           << "\"portfolioId\":" << request.portfolioId << ','
           << "\"instrumentCode\":" << jsonString(request.instrumentCode) << ','
           << "\"side\":" << jsonString(request.side) << ','
           << "\"quantity1e6\":" << request.quantity1e6 << ','
           << "\"reason\":" << jsonString(request.reason) << ','
           << "\"sourceSnapshotId\":" << jsonString(request.sourceSnapshotId) << ','
           << "\"sourceReplayId\":" << jsonString(request.sourceReplayId) << ','
           << "\"riskFlags\":{\"checked\":true,\"blocked\":false},"
           << "\"privacy\":{"
           << "\"rawSqlExposed\":false,"
           << "\"rawTradeLogPayloadExposed\":false,"
           << "\"fullSnapshotPayloadExposed\":false,"
           << "\"internalStackExposed\":false,"
           << "\"sensitiveRawPayloadExposed\":false"
           << "}"
           << "}";
    return stream.str();
}

DatabaseResult<bool> requireReferencedRow(
    SQLiteConnection& connection,
    const char* table,
    std::int64_t id,
    const char* label)
{
    auto rows = connection.queryRows(
        std::string("SELECT id FROM ") + table + " WHERE id = ? LIMIT 1;",
        {std::to_string(id)});
    if (!rows) {
        return DatabaseResult<bool>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            std::string("ShellAccounting TradeDraft requires existing ") + label);
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> propagateFailure(const DatabaseResult<bool>& result)
{
    return DatabaseResult<bool>::failure(result.error().errorCode, result.error().message);
}

}  // namespace

ShellAccountingTradeDraftRepository::ShellAccountingTradeDraftRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<ShellAccountingTradeDraftCreateResult>
ShellAccountingTradeDraftRepository::createTradeDraft(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    auto validation = validateRequest(request);
    if (!validation) {
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
            validation.error().errorCode,
            validation.error().message);
    }

    auto existing = findExistingDraft(request);
    if (!existing) {
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
            existing.error().errorCode,
            existing.error().message);
    }
    if (existing.value().has_value()) {
        ShellAccountingTradeDraftCreateResult result;
        result.draftId = *existing.value();
        result.draftSignature = draftSignature(request);
        result.draftUid = draftUid(request);
        result.duplicateDraft = true;
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::success(result);
    }

    ShellAccountingTradeDraftCreateResult result;
    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto insertedDraft = insertDraft(request);
        if (!insertedDraft) {
            return DatabaseResult<bool>::failure(
                insertedDraft.error().errorCode,
                insertedDraft.error().message);
        }

        result.draftId = insertedDraft.value();
        result.draftUid = draftUid(request);
        result.draftSignature = draftSignature(request);
        result.draftWritten = true;

        auto insertedAudit = insertAuditEvent(request, result.draftId, result.draftUid);
        if (!insertedAudit) {
            return DatabaseResult<bool>::failure(
                insertedAudit.error().errorCode,
                insertedAudit.error().message);
        }

        result.auditLogId = insertedAudit.value();
        result.auditWritten = true;
        return DatabaseResult<bool>::success(true);
    });

    if (!transactionResult) {
        return DatabaseResult<ShellAccountingTradeDraftCreateResult>::failure(
            transactionResult.error().errorCode,
            transactionResult.error().message);
    }

    result.transactionCommitted = true;
    return DatabaseResult<ShellAccountingTradeDraftCreateResult>::success(result);
}

DatabaseResult<bool> ShellAccountingTradeDraftRepository::validateRequest(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    if (request.authorizationToken != "TASK-148_TRADEDRAFT_WRITE") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "ShellAccounting TradeDraft creation requires TASK-148_TRADEDRAFT_WRITE authorization");
    }
    if (request.accountId <= 0 || request.portfolioId <= 0) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires accountId and portfolioId");
    }
    if (request.strategyId <= 0 || isBlank(request.strategyCode)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires strategyId and strategyCode");
    }
    if (request.instrumentId <= 0 || isBlank(request.instrumentCode)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires instrumentId and instrumentCode");
    }
    if (request.side != "BUY" && request.side != "SELL") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft side must be BUY or SELL");
    }
    if (request.quantity1e6 <= 0) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft quantity must be positive");
    }
    if (isBlank(request.reason)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires reason");
    }
    if (isBlank(request.sourceSnapshotId) && isBlank(request.sourceReplayId)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft requires sourceSnapshotId or sourceReplayId");
    }

    auto account = requireReferencedRow(connection_, "account", request.accountId, "account");
    if (!account) {
        return propagateFailure(account);
    }
    auto portfolio = requireReferencedRow(connection_, "portfolio", request.portfolioId, "portfolio");
    if (!portfolio) {
        return propagateFailure(portfolio);
    }
    auto strategy = requireReferencedRow(connection_, "strategy", request.strategyId, "strategy");
    if (!strategy) {
        return propagateFailure(strategy);
    }
    auto instrument = requireReferencedRow(connection_, "instrument", request.instrumentId, "instrument");
    if (!instrument) {
        return propagateFailure(instrument);
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::optional<std::int64_t>>
ShellAccountingTradeDraftRepository::findExistingDraft(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    auto rows = connection_.queryRows(
        "SELECT id FROM trade_draft "
        "WHERE account_id = ? AND portfolio_id = ? AND strategy_code = ? AND draft_signature = ? "
        "ORDER BY id DESC LIMIT 1;",
        {std::to_string(request.accountId),
         std::to_string(request.portfolioId),
         request.strategyCode,
         draftSignature(request)});
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

DatabaseResult<std::int64_t> ShellAccountingTradeDraftRepository::insertDraft(
    const ShellAccountingTradeDraftCreateRequest& request)
{
    const auto uid = draftUid(request);
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_draft("
        "uid,account_id,portfolio_id,strategy_id,strategy_code,action_type,trade_source,"
        "trade_intent_type,trigger_reason,status,total_quantity_1e6,draft_signature,"
        "market_status,created_at_utc,last_seen_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(uid),
         SqlStatementParameter::int64ValueOf(request.accountId),
         SqlStatementParameter::int64ValueOf(request.portfolioId),
         SqlStatementParameter::int64ValueOf(request.strategyId),
         SqlStatementParameter::textValue(request.strategyCode),
         SqlStatementParameter::textValue(request.side),
         SqlStatementParameter::textValue("SYSTEM"),
         SqlStatementParameter::textValue("MANUAL_TEMPLATE"),
         SqlStatementParameter::textValue(request.reason),
         SqlStatementParameter::textValue("ACTIVE"),
         SqlStatementParameter::int64ValueOf(request.quantity1e6),
         SqlStatementParameter::textValue(draftSignature(request)),
         SqlStatementParameter::textValue("UNKNOWN"),
         SqlStatementParameter::textValue(request.createdAtUtc),
         SqlStatementParameter::textValue(request.createdAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(
            inserted.error().errorCode,
            inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftRepository::insertAuditEvent(
    const ShellAccountingTradeDraftCreateRequest& request,
    std::int64_t draftId,
    const std::string& uid)
{
    AuditLogEntry entry;
    entry.accountId = request.accountId;
    entry.portfolioId = request.portfolioId;
    entry.entityType = "SHELL_ACCOUNTING_TRADEDRAFT";
    entry.entityId = draftId;
    entry.action = "ACCOUNTING_TRADEDRAFT_CREATE";
    entry.oldValueJson = "{}";
    entry.newValueJson = sanitizedAuditPayload(request, draftId, uid);
    entry.reason = auditReason(request);
    entry.operatorName = "dataservice";
    entry.createdAtUtc = request.createdAtUtc;

    AuditLogRepository auditLogs(connection_);
    return auditLogs.insertAuditLog(entry);
}

}  // namespace etfdt::data_access
