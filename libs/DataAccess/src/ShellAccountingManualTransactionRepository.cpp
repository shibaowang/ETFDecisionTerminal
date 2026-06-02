#include "DataAccess/ShellAccountingManualTransactionRepository.h"

#include "DataAccess/TransactionRunner.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace etfdt::data_access {
namespace {

bool isBlank(const std::string& value)
{
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
}

std::string upperAscii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

bool containsInsensitive(const std::string& text, const std::string& token)
{
    auto lowerText = text;
    auto lowerToken = token;
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    std::transform(lowerToken.begin(), lowerToken.end(), lowerToken.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return lowerText.find(lowerToken) != std::string::npos;
}

std::string sanitizeMemo(const std::string& memo)
{
    std::string sanitized;
    sanitized.reserve(memo.size());
    for (const char ch : memo) {
        if (ch == '\r' || ch == '\n' || ch == '\t') {
            sanitized.push_back(' ');
        } else {
            sanitized.push_back(ch);
        }
    }
    if (sanitized.size() > 240U) {
        sanitized.resize(240U);
    }
    return sanitized;
}

bool memoHasSensitiveToken(const std::string& memo)
{
    for (const auto& token : {"credential", "secret", "token", "password", "apikey",
             "api_key", "http://", "https://", "select ", "insert "}) {
        if (containsInsensitive(memo, token)) {
            return true;
        }
    }
    return false;
}

std::string uidPart(std::string value)
{
    if (value.empty()) {
        return "none";
    }
    for (char& ch : value) {
        const bool allowed = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_';
        if (!allowed) {
            ch = '-';
        }
    }
    if (value.size() > 96U) {
        value.resize(96U);
    }
    return value;
}

std::string uidSeed(const ShellAccountingManualTransactionWriteCommand& command)
{
    if (!isBlank(command.idempotencyKey)) {
        return uidPart(command.idempotencyKey);
    }
    std::ostringstream stream;
    stream << uidPart(command.requestId) << '-' << command.accountId << '-' << command.portfolioId
           << '-' << uidPart(command.securityCode) << '-' << upperAscii(command.tradeSide)
           << '-' << command.quantityUnits << '-' << command.grossAmountMinor << '-'
           << uidPart(command.occurredAtUtc);
    return uidPart(stream.str());
}

std::string executionGroupUid(const ShellAccountingManualTransactionWriteCommand& command)
{
    return "task-192-manual-transaction-group-" + uidSeed(command);
}

std::string tradeLogUid(const ShellAccountingManualTransactionWriteCommand& command)
{
    return "task-192-manual-transaction-log-" + uidSeed(command);
}

std::string tradeIntentType(const std::string& side)
{
    return upperAscii(side) == "BUY" ? "NORMAL_BUY" : "NORMAL_SELL";
}

std::int64_t netCashImpactCents(const ShellAccountingManualTransactionWriteCommand& command)
{
    const auto side = upperAscii(command.tradeSide);
    const auto total = command.grossAmountMinor + command.feeAmountMinor + command.taxAmountMinor;
    if (side == "BUY") {
        return -total;
    }
    return command.grossAmountMinor - command.feeAmountMinor - command.taxAmountMinor;
}

DatabaseResult<bool> requireRow(
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
            std::string("ShellAccounting manual transaction repository requires existing ") + label);
    }
    return DatabaseResult<bool>::success(true);
}

}  // namespace

ShellAccountingManualTransactionRepository::ShellAccountingManualTransactionRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

ShellAccountingManualTransactionWriteResult ShellAccountingManualTransactionRepository::failure(
    etfdt::protocol::ErrorCode code,
    std::string status,
    std::string issue) const
{
    ShellAccountingManualTransactionWriteResult result;
    result.writeImplemented = true;
    result.errorCode = code;
    result.status = std::move(status);
    result.issues.push_back(std::move(issue));
    return result;
}

ShellAccountingManualTransactionWriteResult ShellAccountingManualTransactionRepository::validate(
    const ShellAccountingManualTransactionWriteCommand& command) const
{
    if (command.accountId <= 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "ACCOUNT_ID_REQUIRED");
    }
    if (command.portfolioId <= 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "PORTFOLIO_ID_REQUIRED");
    }
    if (command.instrumentId <= 0 || isBlank(command.securityCode)) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "SECURITY_CODE_REQUIRED");
    }
    const auto side = upperAscii(command.tradeSide);
    if (side != "BUY" && side != "SELL") {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "TRADE_SIDE_INVALID");
    }
    if (command.quantityUnits <= 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "QUANTITY_POSITIVE_REQUIRED");
    }
    if (command.priceAmountMinor <= 0 || command.grossAmountMinor <= 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "AMOUNT_POSITIVE_REQUIRED");
    }
    if (command.feeAmountMinor < 0 || command.taxAmountMinor < 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "FEE_TAX_NON_NEGATIVE_REQUIRED");
    }
    if (isBlank(command.occurredAtUtc)) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "OCCURRED_AT_REQUIRED");
    }
    if (memoHasSensitiveToken(command.sourceMemo)) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "SENSITIVE_MEMO_VALUE");
    }

    ShellAccountingManualTransactionWriteResult result;
    result.accepted = true;
    result.status = "VALIDATION_ACCEPTED";
    result.requestId = command.requestId;
    result.idempotencyKey = command.idempotencyKey;
    return result;
}

DatabaseResult<ShellAccountingManualTransactionWriteResult>
ShellAccountingManualTransactionRepository::findExisting(
    const ShellAccountingManualTransactionWriteCommand& command)
{
    if (isBlank(command.idempotencyKey)) {
        ShellAccountingManualTransactionWriteResult result;
        result.status = "NO_IDEMPOTENCY_KEY";
        return DatabaseResult<ShellAccountingManualTransactionWriteResult>::success(result);
    }

    auto rows = connection_.queryRows(
        "SELECT tl.id, tl.uid, COALESCE(tl.trade_execution_group_id, 0), "
        "COALESCE(teg.uid, '') "
        "FROM trade_log tl "
        "LEFT JOIN trade_execution_group teg ON teg.id = tl.trade_execution_group_id "
        "WHERE tl.idempotency_key = ? LIMIT 1;",
        {command.idempotencyKey});
    if (!rows) {
        return DatabaseResult<ShellAccountingManualTransactionWriteResult>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty()) {
        ShellAccountingManualTransactionWriteResult result;
        result.status = "NO_DUPLICATE";
        return DatabaseResult<ShellAccountingManualTransactionWriteResult>::success(result);
    }

    const auto& row = rows.value().front();
    ShellAccountingManualTransactionWriteResult result;
    result.accepted = true;
    result.success = true;
    result.writeImplemented = true;
    result.databaseWritten = true;
    result.tradeExecutionGroupWritten = true;
    result.tradeLogWritten = true;
    result.idempotentReplay = true;
    result.duplicate = true;
    result.transactionCommitted = false;
    result.tradeLogId = row[0].int64Value;
    result.tradeLogUid = row[1].text;
    result.tradeExecutionGroupId = row[2].int64Value;
    result.tradeExecutionGroupUid = row[3].text;
    result.requestId = command.requestId;
    result.idempotencyKey = command.idempotencyKey;
    result.status = "IDEMPOTENT_REPLAY";
    return DatabaseResult<ShellAccountingManualTransactionWriteResult>::success(std::move(result));
}

DatabaseResult<bool> ShellAccountingManualTransactionRepository::requireReferencedRows(
    const ShellAccountingManualTransactionWriteCommand& command)
{
    auto account = requireRow(connection_, "account", command.accountId, "account");
    if (!account) {
        return account;
    }
    auto portfolio = requireRow(connection_, "portfolio", command.portfolioId, "portfolio");
    if (!portfolio) {
        return portfolio;
    }
    auto instrument = requireRow(connection_, "instrument", command.instrumentId, "instrument");
    if (!instrument) {
        return instrument;
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::int64_t> ShellAccountingManualTransactionRepository::insertExecutionGroup(
    const ShellAccountingManualTransactionWriteCommand& command,
    const std::string& uid)
{
    const auto side = upperAscii(command.tradeSide);
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_execution_group("
        "uid,account_id,portfolio_id,action_type,trade_source,trade_intent_type,"
        "total_amount_cents,total_quantity_1e6,confirm_mode,manual_entry,manual_reason,"
        "created_at_utc) VALUES(?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(uid),
         SqlStatementParameter::int64ValueOf(command.accountId),
         SqlStatementParameter::int64ValueOf(command.portfolioId),
         SqlStatementParameter::textValue(side),
         SqlStatementParameter::textValue("MANUAL"),
         SqlStatementParameter::textValue(tradeIntentType(side)),
         SqlStatementParameter::int64ValueOf(command.grossAmountMinor),
         SqlStatementParameter::int64ValueOf(command.quantityUnits),
         SqlStatementParameter::textValue("MANUAL_ENTRY"),
         SqlStatementParameter::int64ValueOf(1),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         SqlStatementParameter::textValue(command.occurredAtUtc)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(inserted.error().errorCode, inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::int64_t> ShellAccountingManualTransactionRepository::insertTradeLog(
    const ShellAccountingManualTransactionWriteCommand& command,
    const std::string& uid,
    std::int64_t executionGroupId)
{
    const auto side = upperAscii(command.tradeSide);
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_log("
        "uid,account_id,portfolio_id,trade_execution_group_id,actual_instrument_id,actual_code,"
        "action_type,trade_source,trade_intent_type,price_1e6,quantity_1e6,amount_cents,"
        "fee_cents,net_cash_impact_cents,memo,manual_entry,created_at_utc,request_id,"
        "idempotency_key,occurred_at_utc,tax_cents,source_memo_sanitized,cash_adjustment_uid) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(uid),
         SqlStatementParameter::int64ValueOf(command.accountId),
         SqlStatementParameter::int64ValueOf(command.portfolioId),
         SqlStatementParameter::int64ValueOf(executionGroupId),
         SqlStatementParameter::int64ValueOf(command.instrumentId),
         SqlStatementParameter::textValue(command.securityCode),
         SqlStatementParameter::textValue(side),
         SqlStatementParameter::textValue("MANUAL"),
         SqlStatementParameter::textValue(tradeIntentType(side)),
         SqlStatementParameter::int64ValueOf(command.priceAmountMinor),
         SqlStatementParameter::int64ValueOf(command.quantityUnits),
         SqlStatementParameter::int64ValueOf(command.grossAmountMinor),
         SqlStatementParameter::int64ValueOf(command.feeAmountMinor),
         SqlStatementParameter::int64ValueOf(netCashImpactCents(command)),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         SqlStatementParameter::int64ValueOf(1),
         SqlStatementParameter::textValue(command.occurredAtUtc),
         SqlStatementParameter::textValue(command.requestId),
         isBlank(command.idempotencyKey) ? SqlStatementParameter::nullValue()
                                         : SqlStatementParameter::textValue(command.idempotencyKey),
         SqlStatementParameter::textValue(command.occurredAtUtc),
         SqlStatementParameter::int64ValueOf(command.taxAmountMinor),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         SqlStatementParameter::nullValue()});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(inserted.error().errorCode, inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

ShellAccountingManualTransactionWriteResult
ShellAccountingManualTransactionRepository::persistManualTransaction(
    const ShellAccountingManualTransactionWriteCommand& command)
{
    auto validation = validate(command);
    if (!validation.success && validation.status == "VALIDATION_REJECTED") {
        return validation;
    }

    auto existing = findExisting(command);
    if (!existing) {
        return failure(
            existing.error().errorCode,
            "IDEMPOTENCY_LOOKUP_FAILED",
            "SAFE_SQLITE_ERROR");
    }
    if (existing.value().duplicate) {
        return existing.value();
    }

    auto references = requireReferencedRows(command);
    if (!references) {
        return failure(
            references.error().errorCode,
            "REFERENCE_VALIDATION_FAILED",
            "REFERENCE_ROW_MISSING");
    }

    ShellAccountingManualTransactionWriteResult result = validation;
    result.tradeExecutionGroupUid = executionGroupUid(command);
    result.tradeLogUid = tradeLogUid(command);

    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto group = insertExecutionGroup(command, result.tradeExecutionGroupUid);
        if (!group) {
            return DatabaseResult<bool>::failure(group.error().errorCode, group.error().message);
        }
        result.tradeExecutionGroupId = group.value();
        result.tradeExecutionGroupWritten = true;

        auto log = insertTradeLog(command, result.tradeLogUid, result.tradeExecutionGroupId);
        if (!log) {
            return DatabaseResult<bool>::failure(log.error().errorCode, log.error().message);
        }
        result.tradeLogId = log.value();
        result.tradeLogWritten = true;
        return DatabaseResult<bool>::success(true);
    });

    if (!transactionResult) {
        return failure(
            transactionResult.error().errorCode,
            "TRANSACTION_ROLLED_BACK",
            "SAFE_SQLITE_ERROR");
    }

    result.success = true;
    result.databaseWritten = true;
    result.transactionCommitted = true;
    result.status = "OK";
    return result;
}

}  // namespace etfdt::data_access
