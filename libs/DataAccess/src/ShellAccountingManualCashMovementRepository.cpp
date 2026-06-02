#include "DataAccess/ShellAccountingManualCashMovementRepository.h"

#include "DataAccess/TransactionRunner.h"

#include <algorithm>
#include <atomic>
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
             "api_key", "http://", "https://", "select ", "insert ", "endpoint"}) {
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

std::string uidSeed(const ShellAccountingManualCashMovementWriteCommand& command)
{
    if (!isBlank(command.idempotencyKey)) {
        return uidPart(command.idempotencyKey);
    }
    static std::atomic<std::uint64_t> sequence{0};
    std::ostringstream stream;
    stream << uidPart(command.requestId) << '-' << command.accountId << '-' << command.portfolioId
           << '-' << upperAscii(command.movementType) << '-' << command.amountMinor << '-'
           << uidPart(command.occurredAtUtc) << '-' << sequence.fetch_add(1);
    return uidPart(stream.str());
}

std::string tradeLogUid(const ShellAccountingManualCashMovementWriteCommand& command)
{
    return "task-196-manual-cash-trade-log-" + uidSeed(command);
}

std::string cashAdjustmentUid(const ShellAccountingManualCashMovementWriteCommand& command)
{
    return "task-196-manual-cash-adjustment-" + uidSeed(command);
}

std::string actionType(const std::string& movementType)
{
    return upperAscii(movementType) == "DEPOSIT" ? "CASH_IN" : "CASH_OUT";
}

std::int64_t signedCashAmount(const ShellAccountingManualCashMovementWriteCommand& command)
{
    return upperAscii(command.movementType) == "DEPOSIT" ? command.amountMinor : -command.amountMinor;
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
            std::string("ShellAccounting manual cash movement repository requires existing ") + label);
    }
    return DatabaseResult<bool>::success(true);
}

}  // namespace

ShellAccountingManualCashMovementRepository::ShellAccountingManualCashMovementRepository(
    SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

ShellAccountingManualCashMovementWriteResult ShellAccountingManualCashMovementRepository::failure(
    etfdt::protocol::ErrorCode code,
    std::string status,
    std::string issue) const
{
    ShellAccountingManualCashMovementWriteResult result;
    result.writeImplemented = true;
    result.errorCode = code;
    result.status = std::move(status);
    result.issues.push_back(std::move(issue));
    return result;
}

ShellAccountingManualCashMovementWriteResult ShellAccountingManualCashMovementRepository::validate(
    const ShellAccountingManualCashMovementWriteCommand& command) const
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
    const auto type = upperAscii(command.movementType);
    if (type != "DEPOSIT" && type != "WITHDRAWAL") {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "MOVEMENT_TYPE_INVALID");
    }
    if (command.amountMinor <= 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "VALIDATION_REJECTED",
            "AMOUNT_POSITIVE_REQUIRED");
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

    ShellAccountingManualCashMovementWriteResult result;
    result.accepted = true;
    result.status = "VALIDATION_ACCEPTED";
    result.requestId = command.requestId;
    result.idempotencyKey = command.idempotencyKey;
    return result;
}

DatabaseResult<ShellAccountingManualCashMovementWriteResult>
ShellAccountingManualCashMovementRepository::findExisting(
    const ShellAccountingManualCashMovementWriteCommand& command)
{
    if (isBlank(command.idempotencyKey)) {
        ShellAccountingManualCashMovementWriteResult result;
        result.status = "NO_IDEMPOTENCY_KEY";
        return DatabaseResult<ShellAccountingManualCashMovementWriteResult>::success(result);
    }

    auto rows = connection_.queryRows(
        "SELECT tl.id, tl.uid, ca.id, ca.uid "
        "FROM trade_log tl "
        "JOIN cash_adjustment ca ON ca.trade_log_id = tl.id "
        "WHERE tl.idempotency_key = ? LIMIT 1;",
        {command.idempotencyKey});
    if (!rows) {
        return DatabaseResult<ShellAccountingManualCashMovementWriteResult>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty()) {
        ShellAccountingManualCashMovementWriteResult result;
        result.status = "NO_DUPLICATE";
        return DatabaseResult<ShellAccountingManualCashMovementWriteResult>::success(result);
    }

    const auto& row = rows.value().front();
    ShellAccountingManualCashMovementWriteResult result;
    result.accepted = true;
    result.success = true;
    result.writeImplemented = true;
    result.databaseWritten = true;
    result.tradeLogWritten = true;
    result.cashAdjustmentWritten = true;
    result.idempotentReplay = true;
    result.duplicate = true;
    result.transactionCommitted = false;
    result.tradeLogId = row[0].int64Value;
    result.tradeLogUid = row[1].text;
    result.cashAdjustmentId = row[2].int64Value;
    result.cashAdjustmentUid = row[3].text;
    result.requestId = command.requestId;
    result.idempotencyKey = command.idempotencyKey;
    result.status = "IDEMPOTENT_REPLAY";
    return DatabaseResult<ShellAccountingManualCashMovementWriteResult>::success(std::move(result));
}

DatabaseResult<bool> ShellAccountingManualCashMovementRepository::requireReferencedRows(
    const ShellAccountingManualCashMovementWriteCommand& command)
{
    auto account = requireRow(connection_, "account", command.accountId, "account");
    if (!account) {
        return account;
    }
    auto portfolio = requireRow(connection_, "portfolio", command.portfolioId, "portfolio");
    if (!portfolio) {
        return portfolio;
    }
    if (!isBlank(command.currencyCode)) {
        auto rows = connection_.queryRows(
            "SELECT id FROM account WHERE id = ? AND base_currency = ? LIMIT 1;",
            {std::to_string(command.accountId), command.currencyCode});
        if (!rows) {
            return DatabaseResult<bool>::failure(rows.error().errorCode, rows.error().message);
        }
        if (rows.value().empty()) {
            return DatabaseResult<bool>::failure(
                etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "ShellAccounting manual cash movement repository requires currency matching account base_currency");
        }
    }
    return DatabaseResult<bool>::success(true);
}

DatabaseResult<std::int64_t> ShellAccountingManualCashMovementRepository::insertTradeLog(
    const ShellAccountingManualCashMovementWriteCommand& command,
    const std::string& tradeLogUid,
    const std::string& cashAdjustmentUid)
{
    const auto action = actionType(command.movementType);
    auto inserted = connection_.executeStatement(
        "INSERT INTO trade_log("
        "uid,account_id,portfolio_id,action_type,trade_source,trade_intent_type,"
        "quantity_1e6,amount_cents,fee_cents,net_cash_impact_cents,memo,manual_entry,"
        "created_at_utc,request_id,idempotency_key,occurred_at_utc,tax_cents,"
        "source_memo_sanitized,cash_adjustment_uid) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(tradeLogUid),
         SqlStatementParameter::int64ValueOf(command.accountId),
         SqlStatementParameter::int64ValueOf(command.portfolioId),
         SqlStatementParameter::textValue(action),
         SqlStatementParameter::textValue("MANUAL"),
         SqlStatementParameter::textValue("CASH_EVENT"),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::int64ValueOf(command.amountMinor),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::int64ValueOf(signedCashAmount(command)),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         SqlStatementParameter::int64ValueOf(1),
         SqlStatementParameter::textValue(command.occurredAtUtc),
         SqlStatementParameter::textValue(command.requestId),
         isBlank(command.idempotencyKey) ? SqlStatementParameter::nullValue()
                                         : SqlStatementParameter::textValue(command.idempotencyKey),
         SqlStatementParameter::textValue(command.occurredAtUtc),
         SqlStatementParameter::int64ValueOf(0),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         SqlStatementParameter::textValue(cashAdjustmentUid)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(inserted.error().errorCode, inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

DatabaseResult<std::int64_t> ShellAccountingManualCashMovementRepository::insertCashAdjustment(
    const ShellAccountingManualCashMovementWriteCommand& command,
    const std::string& cashAdjustmentUid,
    const std::string& tradeLogUid,
    std::int64_t tradeLogId)
{
    const auto action = actionType(command.movementType);
    const auto externalReference = isBlank(command.sourceReference)
        ? SqlStatementParameter::nullValue()
        : SqlStatementParameter::textValue(command.sourceReference);
    auto inserted = connection_.executeStatement(
        "INSERT INTO cash_adjustment("
        "uid,trade_log_id,account_id,portfolio_id,adjustment_type,amount_cents,reason,"
        "external_reference,created_at_utc,request_id,idempotency_key,occurred_at_utc,"
        "source_memo_sanitized,trade_log_uid) "
        "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?);",
        {SqlStatementParameter::textValue(cashAdjustmentUid),
         SqlStatementParameter::int64ValueOf(tradeLogId),
         SqlStatementParameter::int64ValueOf(command.accountId),
         SqlStatementParameter::int64ValueOf(command.portfolioId),
         SqlStatementParameter::textValue(action),
         SqlStatementParameter::int64ValueOf(signedCashAmount(command)),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         externalReference,
         SqlStatementParameter::textValue(command.occurredAtUtc),
         SqlStatementParameter::textValue(command.requestId),
         isBlank(command.idempotencyKey) ? SqlStatementParameter::nullValue()
                                         : SqlStatementParameter::textValue(command.idempotencyKey),
         SqlStatementParameter::textValue(command.occurredAtUtc),
         SqlStatementParameter::textValue(sanitizeMemo(command.sourceMemo)),
         SqlStatementParameter::textValue(tradeLogUid)});
    if (!inserted) {
        return DatabaseResult<std::int64_t>::failure(inserted.error().errorCode, inserted.error().message);
    }
    return DatabaseResult<std::int64_t>::success(connection_.lastInsertRowId());
}

ShellAccountingManualCashMovementWriteResult
ShellAccountingManualCashMovementRepository::persistManualCashMovement(
    const ShellAccountingManualCashMovementWriteCommand& command)
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

    ShellAccountingManualCashMovementWriteResult result = validation;
    result.tradeLogUid = tradeLogUid(command);
    result.cashAdjustmentUid = cashAdjustmentUid(command);

    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        auto log = insertTradeLog(command, result.tradeLogUid, result.cashAdjustmentUid);
        if (!log) {
            return DatabaseResult<bool>::failure(log.error().errorCode, log.error().message);
        }
        result.tradeLogId = log.value();
        result.tradeLogWritten = true;

        auto cash = insertCashAdjustment(command, result.cashAdjustmentUid, result.tradeLogUid, result.tradeLogId);
        if (!cash) {
            return DatabaseResult<bool>::failure(cash.error().errorCode, cash.error().message);
        }
        result.cashAdjustmentId = cash.value();
        result.cashAdjustmentWritten = true;
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
