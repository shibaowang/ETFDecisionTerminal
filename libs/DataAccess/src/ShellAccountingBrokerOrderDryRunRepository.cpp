#include "DataAccess/ShellAccountingBrokerOrderDryRunRepository.h"

#include "DataAccess/AuditLogRepository.h"
#include "DataAccess/TransactionRunner.h"
#include "Protocol/Json.h"

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

std::string jsonString(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::string sanitizedAuditPayload(
    const ShellAccountingBrokerOrderDryRunRequest& request,
    const ShellAccountingBrokerOrderDryRunRepository::ConfirmedTradeRow& trade)
{
    std::ostringstream stream;
    stream << "{"
           << "\"sourceAction\":\"accounting.broker_order.dry_run\","
           << "\"authorizationToken\":" << jsonString(request.authorizationToken) << ','
           << "\"draftId\":" << trade.draftId << ','
           << "\"tradeExecutionGroupId\":" << trade.executionGroupId << ','
           << "\"tradeLogId\":" << trade.tradeLogId << ','
           << "\"accountId\":" << trade.accountId << ','
           << "\"portfolioId\":" << trade.portfolioId << ','
           << "\"instrumentCode\":" << jsonString(trade.instrumentCode) << ','
           << "\"side\":" << jsonString(trade.side) << ','
           << "\"quantity1e6\":" << trade.quantity1e6 << ','
           << "\"dryRun\":true,"
           << "\"brokerDisabled\":true,"
           << "\"realOrderPlacement\":false,"
           << "\"brokerOrderId\":null,"
           << "\"privacy\":{"
           << "\"rawSqlExposed\":false,"
           << "\"rawTradeLogPayloadExposed\":false,"
           << "\"fullTradeDraftPayloadExposed\":false,"
           << "\"fullBrokerPayloadExposed\":false,"
           << "\"internalStackExposed\":false"
           << "}"
           << "}";
    return stream.str();
}

DatabaseResult<ShellAccountingBrokerOrderDryRunRepository::ConfirmedTradeRow> parseConfirmedTradeRow(
    const SqlQueryRow& row)
{
    if (row.size() != 9U) {
        return DatabaseResult<ShellAccountingBrokerOrderDryRunRepository::ConfirmedTradeRow>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "ShellAccounting broker dry-run confirmed trade query returned unexpected column count");
    }

    ShellAccountingBrokerOrderDryRunRepository::ConfirmedTradeRow trade;
    trade.draftId = row[0].int64Value;
    trade.accountId = row[1].int64Value;
    trade.portfolioId = row[2].int64Value;
    trade.draftStatus = row[3].text;
    trade.executionGroupId = row[4].int64Value;
    trade.tradeLogId = row[5].int64Value;
    trade.side = row[6].text;
    trade.quantity1e6 = row[7].int64Value;
    trade.instrumentCode = row[8].text;
    return DatabaseResult<ShellAccountingBrokerOrderDryRunRepository::ConfirmedTradeRow>::success(trade);
}

}  // namespace

ShellAccountingBrokerOrderDryRunRepository::ShellAccountingBrokerOrderDryRunRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

DatabaseResult<ShellAccountingBrokerOrderDryRunResult>
ShellAccountingBrokerOrderDryRunRepository::dryRunBrokerOrder(
    const ShellAccountingBrokerOrderDryRunRequest& request)
{
    auto trade = loadConfirmedTrade(request);
    if (!trade) {
        return DatabaseResult<ShellAccountingBrokerOrderDryRunResult>::failure(
            trade.error().errorCode,
            trade.error().message);
    }

    auto validation = validateRequest(request, trade.value());
    if (!validation) {
        return DatabaseResult<ShellAccountingBrokerOrderDryRunResult>::failure(
            validation.error().errorCode,
            validation.error().message);
    }

    auto duplicate = findExistingDryRunAudit(trade.value());
    if (!duplicate) {
        return DatabaseResult<ShellAccountingBrokerOrderDryRunResult>::failure(
            duplicate.error().errorCode,
            duplicate.error().message);
    }

    ShellAccountingBrokerOrderDryRunResult result;
    result.draftId = trade.value().draftId;
    result.executionGroupId = trade.value().executionGroupId;
    result.tradeLogId = trade.value().tradeLogId;
    if (duplicate.value().has_value()) {
        result.auditLogId = *duplicate.value();
        result.duplicateDryRun = true;
        return DatabaseResult<ShellAccountingBrokerOrderDryRunResult>::success(result);
    }

    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto auditId = insertAuditEvent(request, trade.value());
        if (!auditId) {
            return DatabaseResult<bool>::failure(auditId.error().errorCode, auditId.error().message);
        }
        result.auditLogId = auditId.value();
        result.auditWritten = true;
        return DatabaseResult<bool>::success(true);
    });
    if (!transactionResult) {
        return DatabaseResult<ShellAccountingBrokerOrderDryRunResult>::failure(
            transactionResult.error().errorCode,
            transactionResult.error().message);
    }

    result.transactionCommitted = true;
    return DatabaseResult<ShellAccountingBrokerOrderDryRunResult>::success(result);
}

DatabaseResult<ShellAccountingBrokerOrderDryRunRepository::ConfirmedTradeRow>
ShellAccountingBrokerOrderDryRunRepository::loadConfirmedTrade(
    const ShellAccountingBrokerOrderDryRunRequest& request)
{
    if (request.draftId <= 0 && request.tradeLogId <= 0) {
        return DatabaseResult<ConfirmedTradeRow>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run requires confirmed draftId or tradeLogId");
    }

    std::string sql =
        "SELECT td.id, td.account_id, td.portfolio_id, td.status, teg.id, tl.id, "
        "td.action_type, td.total_quantity_1e6, COALESCE(tl.actual_code, '') "
        "FROM trade_draft td "
        "JOIN trade_execution_group teg ON teg.draft_id = td.id "
        "JOIN trade_log tl ON tl.trade_execution_group_id = teg.id ";
    std::vector<std::string> parameters;
    if (request.draftId > 0) {
        sql += "WHERE td.id = ? ";
        parameters.push_back(std::to_string(request.draftId));
    }
    else {
        sql += "WHERE tl.id = ? ";
        parameters.push_back(std::to_string(request.tradeLogId));
    }
    sql += "ORDER BY teg.id DESC, tl.id DESC LIMIT 1;";

    auto rows = connection_.queryRows(sql, parameters);
    if (!rows) {
        return DatabaseResult<ConfirmedTradeRow>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty()) {
        return DatabaseResult<ConfirmedTradeRow>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run requires an existing confirmed TradeDraft ledger fact");
    }
    return parseConfirmedTradeRow(rows.value().front());
}

DatabaseResult<bool> ShellAccountingBrokerOrderDryRunRepository::validateRequest(
    const ShellAccountingBrokerOrderDryRunRequest& request,
    const ConfirmedTradeRow& trade)
{
    if (request.authorizationToken != "TASK-154_BROKER_ORDER_DRY_RUN") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8001_AUTH_REQUIRED,
            "ShellAccounting broker dry-run requires TASK-154_BROKER_ORDER_DRY_RUN authorization");
    }
    if (!request.userConfirmed) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "ShellAccounting broker dry-run requires explicit user confirmation");
    }
    if (request.source != "confirmedTradeDraft" && request.source != "confirmedLedgerFact") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run requires source=confirmedTradeDraft or source=confirmedLedgerFact");
    }
    if (trade.draftStatus != "CONFIRMED") {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E8002_PERMISSION_DENIED,
            "ShellAccounting broker dry-run requires confirmed TradeDraft status");
    }
    if (request.accountId > 0 && request.accountId != trade.accountId) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run account does not match confirmed trade");
    }
    if (request.portfolioId > 0 && request.portfolioId != trade.portfolioId) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run portfolio does not match confirmed trade");
    }
    if (trade.tradeLogId <= 0 || trade.executionGroupId <= 0) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run requires confirmed ledger ids");
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::optional<std::int64_t>>
ShellAccountingBrokerOrderDryRunRepository::findExistingDryRunAudit(
    const ConfirmedTradeRow& trade)
{
    auto rows = connection_.queryRows(
        "SELECT id FROM audit_log "
        "WHERE entity_type = 'SHELL_ACCOUNTING_BROKER_ORDER_DRY_RUN' "
        "AND entity_id = ? AND action = 'ACCOUNTING_BROKER_ORDER_DRY_RUN' "
        "ORDER BY id DESC LIMIT 1;",
        {std::to_string(trade.tradeLogId)});
    if (!rows) {
        return DatabaseResult<std::optional<std::int64_t>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty()) {
        return DatabaseResult<std::optional<std::int64_t>>::success(std::nullopt);
    }
    return DatabaseResult<std::optional<std::int64_t>>::success(rows.value().front().front().int64Value);
}

DatabaseResult<std::int64_t> ShellAccountingBrokerOrderDryRunRepository::insertAuditEvent(
    const ShellAccountingBrokerOrderDryRunRequest& request,
    const ConfirmedTradeRow& trade)
{
    if (isBlank(request.requestedAtUtc)) {
        return DatabaseResult<std::int64_t>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ShellAccounting broker dry-run requires requestedAtUtc");
    }

    AuditLogEntry entry;
    entry.accountId = trade.accountId;
    entry.portfolioId = trade.portfolioId;
    entry.entityType = "SHELL_ACCOUNTING_BROKER_ORDER_DRY_RUN";
    entry.entityId = trade.tradeLogId;
    entry.action = "ACCOUNTING_BROKER_ORDER_DRY_RUN";
    entry.oldValueJson = "{}";
    entry.newValueJson = sanitizedAuditPayload(request, trade);
    entry.reason = "ShellAccounting broker order dry-run boundary";
    entry.operatorName = "dataservice";
    entry.createdAtUtc = request.requestedAtUtc;

    AuditLogRepository auditLogs(connection_);
    return auditLogs.insertAuditLog(entry);
}

}  // namespace etfdt::data_access
