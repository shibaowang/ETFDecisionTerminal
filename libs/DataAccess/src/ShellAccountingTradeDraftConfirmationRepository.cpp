#include "DataAccess/ShellAccountingTradeDraftConfirmationRepository.h"

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

std::string sourceId(const ShellAccountingTradeDraftConfirmationRequest& request)
{
    if (!isBlank(request.sourceSnapshotId)) {
        return request.sourceSnapshotId;
    }
    return request.sourceReplayId;
}

std::string expectedDraftSignature(const ShellAccountingTradeDraftConfirmationRequest& request)
{
    std::ostringstream stream;
    stream << "TASK-148|" << request.accountId << '|' << request.portfolioId << '|'
           << request.strategyCode << '|' << request.instrumentCode << '|' << request.side << '|'
           << request.quantity1e6 << '|' << sourceId(request);
    return stream.str();
}

std::string executionGroupUid(const ShellAccountingTradeDraftConfirmationRequest& request)
{
    std::ostringstream stream;
    stream << "task-150-execution-group-" << request.draftId << '-'
           << sanitizeUidPart(expectedDraftSignature(request));
    return stream.str();
}

std::string tradeLogUid(const ShellAccountingTradeDraftConfirmationRequest& request)
{
    std::ostringstream stream;
    stream << "task-150-trade-log-" << request.draftId << '-'
           << sanitizeUidPart(expectedDraftSignature(request));
    return stream.str();
}

std::string tradeIntentType(const std::string& side)
{
    return side == "BUY" ? "NORMAL_BUY" : "NORMAL_SELL";
}

std::int64_t netCashImpactCents(const std::string& side, std::int64_t amountCents)
{
    if (amountCents == 0) {
        return 0;
    }
    return side == "BUY" ? -amountCents : amountCents;
}

std::string auditReason(const ShellAccountingTradeDraftConfirmationRequest& request)
{
    return "ShellAccounting TradeDraft confirmation: " + expectedDraftSignature(request);
}

std::string sanitizedAuditPayload(
    const ShellAccountingTradeDraftConfirmationRequest& request,
    std::int64_t executionGroupId,
    std::int64_t tradeLogId)
{
    std::ostringstream stream;
    stream << "{"
           << "\"sourceAction\":\"accounting.tradedraft.confirm\","
           << "\"authorizationToken\":" << jsonString(request.authorizationToken) << ','
           << "\"draftId\":" << request.draftId << ','
           << "\"tradeExecutionGroupId\":" << executionGroupId << ','
           << "\"tradeLogId\":" << tradeLogId << ','
           << "\"accountId\":" << request.accountId << ','
           << "\"portfolioId\":" << request.portfolioId << ','
           << "\"instrumentCode\":" << jsonString(request.instrumentCode) << ','
           << "\"side\":" << jsonString(request.side) << ','
           << "\"quantity1e6\":" << request.quantity1e6 << ','
           << "\"confirmMode\":\"DRAFT_CONFIRM\","
           << "\"privacy\":{"
           << "\"rawSqlExposed\":false,"
           << "\"rawTradeLogPayloadExposed\":false,"
           << "\"fullDraftPayloadExposed\":false,"
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
            std::string("ShellAccounting TradeDraft confirmation requires existing ") + label);
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> propagateFailure(const DatabaseResult<bool>& result)
{
    return DatabaseResult<bool>::failure(result.error().errorCode, result.error().message);
}

}  // namespace

ShellAccountingTradeDraftConfirmationRepository::ShellAccountingTradeDraftConfirmationRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<ShellAccountingTradeDraftConfirmationResult>
ShellAccountingTradeDraftConfirmationRepository::confirmTradeDraft(
    const ShellAccountingTradeDraftConfirmationRequest& request)
{
    auto draft = loadDraft(request);
    if (!draft) {
        return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::failure(
            draft.error().errorCode,
            draft.error().message);
    }
    if (request.authorizationToken != "TASK-150_TRADEDRAFT_CONFIRM") {
        return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::failure(
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "ShellAccounting TradeDraft confirmation requires TASK-150_TRADEDRAFT_CONFIRM authorization");
    }

    auto duplicate = findExistingConfirmation(request);
    if (!duplicate) {
        return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::failure(
            duplicate.error().errorCode,
            duplicate.error().message);
    }
    if (duplicate.value().has_value()) {
        const bool matchesDraft = request.accountId == draft.value().accountId
            && request.portfolioId == draft.value().portfolioId
            && request.strategyId == draft.value().strategyId
            && request.strategyCode == draft.value().strategyCode
            && request.side == draft.value().actionType
            && request.quantity1e6 == draft.value().totalQuantity1e6
            && draft.value().draftSignature == expectedDraftSignature(request);
        if (!matchesDraft || draft.value().status != "CONFIRMED") {
            return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::failure(
                etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "ShellAccounting TradeDraft confirmation rejected invalid duplicate draft payload");
        }
        auto result = *duplicate.value();
        result.duplicateConfirmation = true;
        result.idempotent = true;
        return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::success(result);
    }

    auto validation = validateRequest(request, draft.value());
    if (!validation) {
        return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::failure(
            validation.error().errorCode,
            validation.error().message);
    }

    ShellAccountingTradeDraftConfirmationResult result;
    result.draftId = request.draftId;
    result.executionGroupUid = executionGroupUid(request);
    result.tradeLogUid = tradeLogUid(request);

    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto insertedGroup = insertExecutionGroup(request, draft.value());
        if (!insertedGroup) {
            return DatabaseResult<bool>::failure(
                insertedGroup.error().errorCode,
                insertedGroup.error().message);
        }
        result.executionGroupId = insertedGroup.value();
        result.executionGroupWritten = true;

        auto insertedLog = insertTradeLog(request, draft.value(), result.executionGroupId);
        if (!insertedLog) {
            return DatabaseResult<bool>::failure(
                insertedLog.error().errorCode,
                insertedLog.error().message);
        }
        result.tradeLogId = insertedLog.value();
        result.tradeLogWritten = true;

        auto updatedDraft = updateDraftStatus(request);
        if (!updatedDraft) {
            return DatabaseResult<bool>::failure(
                updatedDraft.error().errorCode,
                updatedDraft.error().message);
        }
        result.draftStatusUpdated = true;

        auto insertedAudit = insertAuditEvent(request, result.executionGroupId, result.tradeLogId);
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
        return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::failure(
            transactionResult.error().errorCode,
            transactionResult.error().message);
    }

    result.transactionCommitted = true;
    return DatabaseResult<ShellAccountingTradeDraftConfirmationResult>::success(result);
}

DatabaseResult<ShellAccountingTradeDraftConfirmationRepository::DraftRow>
ShellAccountingTradeDraftConfirmationRepository::loadDraft(
    const ShellAccountingTradeDraftConfirmationRequest& request)
{
    if (request.draftId <= 0) {
        return DatabaseResult<DraftRow>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft confirmation requires draftId");
    }

    auto rows = connection_.queryRows(
        "SELECT id, account_id, portfolio_id, strategy_id, strategy_code, action_type, status, "
        "total_amount_cents, total_quantity_1e6, draft_signature, COALESCE(expires_at_utc, '') "
        "FROM trade_draft WHERE id = ? LIMIT 1;",
        {std::to_string(request.draftId)});
    if (!rows) {
        return DatabaseResult<DraftRow>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().size() < 11U) {
        return DatabaseResult<DraftRow>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft confirmation draft missing");
    }

    const auto& row = rows.value().front();
    DraftRow draft;
    draft.id = row[0].int64Value;
    draft.accountId = row[1].int64Value;
    draft.portfolioId = row[2].int64Value;
    draft.strategyId = row[3].int64Value;
    draft.strategyCode = row[4].text;
    draft.actionType = row[5].text;
    draft.status = row[6].text;
    draft.totalAmountCents = row[7].int64Value;
    draft.totalQuantity1e6 = row[8].int64Value;
    draft.draftSignature = row[9].text;
    draft.expiresAtUtc = row[10].text;
    return DatabaseResult<DraftRow>::success(std::move(draft));
}

DatabaseResult<bool> ShellAccountingTradeDraftConfirmationRepository::validateRequest(
    const ShellAccountingTradeDraftConfirmationRequest& request,
    const DraftRow& draft)
{
    if (request.authorizationToken != "TASK-150_TRADEDRAFT_CONFIRM") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "ShellAccounting TradeDraft confirmation requires TASK-150_TRADEDRAFT_CONFIRM authorization");
    }
    if (draft.status == "CONFIRMED") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "ShellAccounting TradeDraft confirmation rejected: draft already confirmed");
    }
    if (draft.status == "CANCELED") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "ShellAccounting TradeDraft confirmation rejected: draft canceled");
    }
    if (draft.status == "EXPIRED" || (!isBlank(draft.expiresAtUtc) && draft.expiresAtUtc <= request.confirmedAtUtc)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "ShellAccounting TradeDraft confirmation rejected: stale draft");
    }
    if (draft.status != "ACTIVE") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "ShellAccounting TradeDraft confirmation requires ACTIVE draft");
    }
    if (request.accountId != draft.accountId || request.portfolioId != draft.portfolioId
        || request.strategyId != draft.strategyId || request.strategyCode != draft.strategyCode
        || request.side != draft.actionType || request.quantity1e6 != draft.totalQuantity1e6
        || draft.draftSignature != expectedDraftSignature(request)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft confirmation rejected invalid draft payload");
    }
    if (request.instrumentId <= 0 || isBlank(request.instrumentCode)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft confirmation requires instrument identity");
    }
    auto instrument = requireReferencedRow(connection_, "instrument", request.instrumentId, "instrument");
    if (!instrument) {
        return propagateFailure(instrument);
    }
    if (isBlank(request.sourceSnapshotId) && isBlank(request.sourceReplayId)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting TradeDraft confirmation requires sourceSnapshotId or sourceReplayId");
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::optional<ShellAccountingTradeDraftConfirmationResult>>
ShellAccountingTradeDraftConfirmationRepository::findExistingConfirmation(
    const ShellAccountingTradeDraftConfirmationRequest& request)
{
    auto rows = connection_.queryRows(
        "SELECT teg.id, tl.id, COALESCE(al.id, 0), teg.uid, tl.uid "
        "FROM trade_execution_group teg "
        "JOIN trade_log tl ON tl.trade_execution_group_id = teg.id "
        "LEFT JOIN audit_log al ON al.entity_type = 'SHELL_ACCOUNTING_TRADEDRAFT_CONFIRMATION' "
        "    AND al.entity_id = teg.id AND al.action = 'ACCOUNTING_TRADEDRAFT_CONFIRM' "
        "WHERE teg.draft_id = ? "
        "ORDER BY teg.id DESC LIMIT 1;",
        {std::to_string(request.draftId)});
    if (!rows) {
        return DatabaseResult<std::optional<ShellAccountingTradeDraftConfirmationResult>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().size() < 5U) {
        return DatabaseResult<std::optional<ShellAccountingTradeDraftConfirmationResult>>::success(std::nullopt);
    }

    ShellAccountingTradeDraftConfirmationResult result;
    const auto& row = rows.value().front();
    result.draftId = request.draftId;
    result.executionGroupId = row[0].int64Value;
    result.tradeLogId = row[1].int64Value;
    result.auditLogId = row[2].int64Value;
    result.executionGroupUid = row[3].text;
    result.tradeLogUid = row[4].text;
    result.duplicateConfirmation = true;
    return DatabaseResult<std::optional<ShellAccountingTradeDraftConfirmationResult>>::success(result);
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftConfirmationRepository::insertExecutionGroup(
    const ShellAccountingTradeDraftConfirmationRequest& request,
    const DraftRow& draft)
{
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_execution_group("
        "uid,account_id,portfolio_id,strategy_id,strategy_code,draft_id,original_draft_id,"
        "action_type,trade_source,trade_intent_type,total_amount_cents,total_quantity_1e6,"
        "confirm_mode,manual_entry,manual_reason,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(executionGroupUid(request)),
         SqlStatementParameter::int64ValueOf(request.accountId),
         SqlStatementParameter::int64ValueOf(request.portfolioId),
         SqlStatementParameter::int64ValueOf(request.strategyId),
         SqlStatementParameter::textValue(request.strategyCode),
         SqlStatementParameter::int64ValueOf(request.draftId),
         SqlStatementParameter::int64ValueOf(request.draftId),
         SqlStatementParameter::textValue(request.side),
         SqlStatementParameter::textValue("MANUAL"),
         SqlStatementParameter::textValue(tradeIntentType(request.side)),
         SqlStatementParameter::int64ValueOf(draft.totalAmountCents),
         SqlStatementParameter::int64ValueOf(draft.totalQuantity1e6),
         SqlStatementParameter::textValue("DRAFT_CONFIRM"),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::textValue("ShellAccounting TradeDraft confirmation"),
         SqlStatementParameter::textValue(request.confirmedAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(inserted.error().errorCode, inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftConfirmationRepository::insertTradeLog(
    const ShellAccountingTradeDraftConfirmationRequest& request,
    const DraftRow& draft,
    std::int64_t executionGroupId)
{
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_log("
        "uid,account_id,portfolio_id,trade_execution_group_id,strategy_id,strategy_code,"
        "actual_instrument_id,actual_code,action_type,trade_source,trade_intent_type,"
        "quantity_1e6,amount_cents,fee_cents,net_cash_impact_cents,memo,manual_entry,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(tradeLogUid(request)),
         SqlStatementParameter::int64ValueOf(request.accountId),
         SqlStatementParameter::int64ValueOf(request.portfolioId),
         SqlStatementParameter::int64ValueOf(executionGroupId),
         SqlStatementParameter::int64ValueOf(request.strategyId),
         SqlStatementParameter::textValue(request.strategyCode),
         SqlStatementParameter::int64ValueOf(request.instrumentId),
         SqlStatementParameter::textValue(request.instrumentCode),
         SqlStatementParameter::textValue(request.side),
         SqlStatementParameter::textValue("MANUAL"),
         SqlStatementParameter::textValue(tradeIntentType(request.side)),
         SqlStatementParameter::int64ValueOf(draft.totalQuantity1e6),
         SqlStatementParameter::int64ValueOf(draft.totalAmountCents),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::int64ValueOf(netCashImpactCents(request.side, draft.totalAmountCents)),
         SqlStatementParameter::textValue("ShellAccounting TradeDraft confirmation ledger fact"),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::textValue(request.confirmedAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(inserted.error().errorCode, inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<bool> ShellAccountingTradeDraftConfirmationRepository::updateDraftStatus(
    const ShellAccountingTradeDraftConfirmationRequest& request)
{
    auto updated = connection_.executeStatement(
        "UPDATE trade_draft SET status = ?, confirmed_at_utc = ?, last_seen_at_utc = ? "
        "WHERE id = ? AND status = ?;",
        {SqlStatementParameter::textValue("CONFIRMED"),
         SqlStatementParameter::textValue(request.confirmedAtUtc),
         SqlStatementParameter::textValue(request.confirmedAtUtc),
         SqlStatementParameter::int64ValueOf(request.draftId),
         SqlStatementParameter::textValue("ACTIVE")});
    if (!updated) {
        return updated;
    }

    auto rows = connection_.queryRows(
        "SELECT status FROM trade_draft WHERE id = ? LIMIT 1;",
        {std::to_string(request.draftId)});
    if (!rows) {
        return DatabaseResult<bool>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty() || rows.value().front().front().text != "CONFIRMED") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "ShellAccounting TradeDraft confirmation failed to update draft status");
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::int64_t> ShellAccountingTradeDraftConfirmationRepository::insertAuditEvent(
    const ShellAccountingTradeDraftConfirmationRequest& request,
    std::int64_t executionGroupId,
    std::int64_t tradeLogId)
{
    AuditLogEntry entry;
    entry.accountId = request.accountId;
    entry.portfolioId = request.portfolioId;
    entry.entityType = "SHELL_ACCOUNTING_TRADEDRAFT_CONFIRMATION";
    entry.entityId = executionGroupId;
    entry.action = "ACCOUNTING_TRADEDRAFT_CONFIRM";
    entry.oldValueJson = "{}";
    entry.newValueJson = sanitizedAuditPayload(request, executionGroupId, tradeLogId);
    entry.reason = auditReason(request);
    entry.operatorName = "dataservice";
    entry.createdAtUtc = request.confirmedAtUtc;

    AuditLogRepository auditLogs(connection_);
    return auditLogs.insertAuditLog(entry);
}

}  // namespace etfdt::data_access
