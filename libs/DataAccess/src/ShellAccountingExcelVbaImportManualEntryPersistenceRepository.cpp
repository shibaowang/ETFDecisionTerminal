#include "DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h"

#include "DataAccess/AuditLogRepository.h"
#include "DataAccess/ShellAccountingManualTransactionRepository.h"
#include "DataAccess/TransactionRunner.h"

#include <algorithm>
#include <cctype>
#include <optional>
#include <sstream>
#include <string_view>

namespace etfdt::data_access {
namespace {

bool isBlank(const std::string& value)
{
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isspace(ch) != 0;
    });
}

std::string trimCopy(std::string_view value)
{
    std::size_t first = 0;
    while (first < value.size() && std::isspace(static_cast<unsigned char>(value[first])) != 0) {
        ++first;
    }
    std::size_t last = value.size();
    while (last > first && std::isspace(static_cast<unsigned char>(value[last - 1U])) != 0) {
        --last;
    }
    return std::string(value.substr(first, last - first));
}

std::string lowerAscii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string upperAscii(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

bool containsSensitiveToken(const std::string& value)
{
    const auto lowered = lowerAscii(value);
    for (const auto* token : {
             "credential",
             "password",
             "secret",
             "token=",
             "api_key",
             "apikey",
             "bearer ",
             "endpoint",
             "http://",
             "https://",
             "select ",
             "insert ",
             "update ",
             "delete ",
             "replace ",
         }) {
        if (lowered.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool safeIdentifierText(const std::string& value)
{
    if (isBlank(value) || value.size() > 160U) {
        return false;
    }
    return std::all_of(value.begin(), value.end(), [](unsigned char ch) {
        return std::isalnum(ch) != 0 || ch == '-' || ch == '_' || ch == ':' || ch == '.';
    });
}

std::string uidPart(std::string value)
{
    for (char& ch : value) {
        const bool allowed = (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')
            || (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == ':' || ch == '.';
        if (!allowed) {
            ch = '-';
        }
    }
    if (value.size() > 96U) {
        value.resize(96U);
    }
    return value.empty() ? "none" : value;
}

std::string jsonString(std::string_view value)
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

std::optional<std::int64_t> parseScaledDecimal(std::string_view value, int scale)
{
    const auto text = trimCopy(value);
    if (text.empty() || scale <= 0) {
        return std::nullopt;
    }

    std::int64_t whole = 0;
    std::int64_t fractional = 0;
    int fractionalDigits = 0;
    bool seenDigit = false;
    bool seenDot = false;
    bool negative = false;
    std::size_t index = 0;
    if (text[index] == '-') {
        negative = true;
        ++index;
    } else if (text[index] == '+') {
        ++index;
    }

    for (; index < text.size(); ++index) {
        const char ch = text[index];
        if (ch == '.') {
            if (seenDot) {
                return std::nullopt;
            }
            seenDot = true;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(ch)) == 0) {
            return std::nullopt;
        }
        seenDigit = true;
        const int digit = ch - '0';
        if (!seenDot) {
            whole = whole * 10 + digit;
        } else if (fractionalDigits < 9) {
            fractional = fractional * 10 + digit;
            ++fractionalDigits;
        }
    }
    if (!seenDigit) {
        return std::nullopt;
    }

    int scaleDigits = 0;
    for (int tmp = scale; tmp > 1; tmp /= 10) {
        ++scaleDigits;
    }
    while (fractionalDigits < scaleDigits) {
        fractional *= 10;
        ++fractionalDigits;
    }
    while (fractionalDigits > scaleDigits) {
        fractional /= 10;
        --fractionalDigits;
    }

    const auto scaled = whole * scale + fractional;
    return negative ? -scaled : scaled;
}

std::string importAuditReason(const std::string& idempotencyKey)
{
    return "ShellAccounting Excel/VBA import manual entry persistence: " + uidPart(idempotencyKey);
}

std::string auditNewValueJson(
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request,
    int tradeLogRowsWritten)
{
    std::ostringstream stream;
    stream << "{"
           << "\"task\":\"TASK-265\","
           << "\"sourceAction\":\"accounting.excel_vba_import.persist_manual_entry\","
           << "\"previewDigest\":" << jsonString(request.previewDigest) << ','
           << "\"idempotencyKey\":" << jsonString(request.idempotencyKey) << ','
           << "\"requestId\":" << jsonString(request.requestId) << ','
           << "\"schemaVersion\":" << jsonString(request.schemaVersion) << ','
           << "\"source\":" << jsonString(request.source) << ','
           << "\"importBatchLabel\":" << jsonString(request.importBatchLabel) << ','
           << "\"rowsWritten\":{\"trade_log\":" << tradeLogRowsWritten << "},"
           << "\"factSummary\":{"
           << "\"tradeFactCount\":" << request.expectedTradeFactCount << ','
           << "\"cashFactCount\":" << request.expectedCashFactCount << ','
           << "\"marketPriceFactCount\":" << request.expectedMarketPriceFactCount << ','
           << "\"fxRateFactCount\":" << request.expectedFxRateFactCount
           << "},"
           << "\"privacy\":{"
           << "\"rawPayloadExposed\":false,"
           << "\"rawSqlExposed\":false,"
           << "\"credentialsExposed\":false,"
           << "\"endpointExposed\":false,"
           << "\"internalStackExposed\":false"
           << "}"
           << "}";
    return stream.str();
}

bool auditJsonHasDigest(const std::string& auditJson, const std::string& previewDigest)
{
    return auditJson.find("\"previewDigest\":" + jsonString(previewDigest)) != std::string::npos;
}

ShellAccountingManualTransactionWriteCommand commandFromFact(
    const ShellAccountingExcelVbaImportManualEntryTradeFact& fact,
    std::int64_t accountId,
    std::int64_t portfolioId,
    std::int64_t instrumentId,
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request,
    std::size_t index)
{
    ShellAccountingManualTransactionWriteCommand command;
    command.accountId = accountId;
    command.portfolioId = portfolioId;
    command.instrumentId = instrumentId;
    command.securityCode = fact.instrumentCode;
    command.tradeSide = upperAscii(fact.side);
    command.quantityUnits = parseScaledDecimal(fact.quantityText, 1000000).value_or(0);
    command.priceAmountMinor = parseScaledDecimal(fact.priceText, 1000000).value_or(0);
    command.grossAmountMinor = parseScaledDecimal(fact.amountText, 100).value_or(0);
    command.feeAmountMinor = parseScaledDecimal(fact.feeText, 100).value_or(0);
    command.taxAmountMinor = 0;
    command.occurredAtUtc = fact.tradeTimeUtc;
    command.sourceMemo = fact.memo.empty() ? "Excel/VBA import manual entry" : fact.memo;
    command.requestId = request.requestId + ":" + uidPart(fact.factId.empty() ? std::to_string(index) : fact.factId);
    command.idempotencyKey =
        request.idempotencyKey + ":trade:" + uidPart(fact.factId.empty() ? std::to_string(index) : fact.factId);
    return command;
}

}  // namespace

ShellAccountingExcelVbaImportManualEntryPersistenceRepository::
    ShellAccountingExcelVbaImportManualEntryPersistenceRepository(SQLiteConnection& connection) noexcept
    : connection_(connection)
{
}

ShellAccountingExcelVbaImportManualEntryPersistenceResult
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::failure(
    etfdt::protocol::ErrorCode code,
    std::string status,
    std::string issue,
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request) const
{
    ShellAccountingExcelVbaImportManualEntryPersistenceResult result;
    result.writeImplemented = true;
    result.previewDigest = request.previewDigest;
    result.idempotencyKey = request.idempotencyKey;
    result.requestId = request.requestId;
    result.errorCode = code;
    result.status = std::move(status);
    result.issues.push_back(std::move(issue));
    return result;
}

ShellAccountingExcelVbaImportManualEntryPersistenceResult
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::validate(
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request) const
{
    if (request.previewStatus != "ACCEPTED") {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "PREVIEW_NOT_ACCEPTED",
            "EXCEL_VBA_IMPORT_PREVIEW_NOT_ACCEPTED",
            request);
    }
    if (!safeIdentifierText(request.previewDigest)) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "DIGEST_REQUIRED",
            "EXCEL_VBA_IMPORT_PREVIEW_DIGEST_REQUIRED",
            request);
    }
    if (!safeIdentifierText(request.idempotencyKey)) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "IDEMPOTENCY_REQUIRED",
            "EXCEL_VBA_IMPORT_IDEMPOTENCY_REQUIRED",
            request);
    }
    if (request.schemaVersion != "excel-vba-export/v1" ||
        request.source != "sanitized-excel-vba-export") {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "SCHEMA_SOURCE_REJECTED",
            "EXCEL_VBA_IMPORT_SCHEMA_SOURCE_REJECTED",
            request);
    }
    if (request.expectedTradeFactCount != static_cast<int>(request.tradeFacts.size()) ||
        request.expectedTradeFactCount <= 0) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "FACT_SUMMARY_MISMATCH",
            "EXCEL_VBA_IMPORT_FACT_SUMMARY_MISMATCH",
            request);
    }
    if (containsSensitiveToken(request.importBatchLabel) || containsSensitiveToken(request.requestId)) {
        return failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "SENSITIVE_VALUE_REJECTED",
            "EXCEL_VBA_IMPORT_SENSITIVE_VALUE_REJECTED",
            request);
    }
    for (const auto& fact : request.tradeFacts) {
        if (isBlank(fact.factId) || isBlank(fact.tradeTimeUtc) || isBlank(fact.accountCode) ||
            isBlank(fact.portfolioCode) || isBlank(fact.instrumentCode) || isBlank(fact.side)) {
            return failure(
                etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "TRADE_FACT_REQUIRED_FIELD_MISSING",
                "EXCEL_VBA_IMPORT_TRADE_FACT_REQUIRED_FIELD_MISSING",
                request);
        }
        if (containsSensitiveToken(fact.source) || containsSensitiveToken(fact.memo)) {
            return failure(
                etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "SENSITIVE_VALUE_REJECTED",
                "EXCEL_VBA_IMPORT_SENSITIVE_VALUE_REJECTED",
                request);
        }
        if (!parseScaledDecimal(fact.quantityText, 1000000).has_value() ||
            !parseScaledDecimal(fact.priceText, 1000000).has_value() ||
            !parseScaledDecimal(fact.amountText, 100).has_value() ||
            !parseScaledDecimal(fact.feeText, 100).has_value()) {
            return failure(
                etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "TRADE_FACT_NUMERIC_FIELD_INVALID",
                "EXCEL_VBA_IMPORT_TRADE_FACT_NUMERIC_FIELD_INVALID",
                request);
        }
    }

    ShellAccountingExcelVbaImportManualEntryPersistenceResult result;
    result.accepted = true;
    result.previewDigest = request.previewDigest;
    result.idempotencyKey = request.idempotencyKey;
    result.requestId = request.requestId;
    result.status = "VALIDATION_ACCEPTED";
    return result;
}

DatabaseResult<std::int64_t>
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::findIdByUid(
    const char* tableName,
    const std::string& uid,
    const char* label)
{
    auto rows = connection_.queryRows(
        std::string("SELECT id FROM ") + tableName + " WHERE uid = ? LIMIT 1;",
        {uid});
    if (!rows) {
        return DatabaseResult<std::int64_t>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty()) {
        return DatabaseResult<std::int64_t>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            std::string("Missing referenced ") + label);
    }
    return DatabaseResult<std::int64_t>::success(rows.value().front().front().int64Value);
}

DatabaseResult<std::int64_t>
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::findInstrumentIdByCode(
    const std::string& code)
{
    auto rows = connection_.queryRows(
        "SELECT id FROM instrument WHERE code = ? LIMIT 1;",
        {code});
    if (!rows) {
        return DatabaseResult<std::int64_t>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty()) {
        return DatabaseResult<std::int64_t>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "Missing referenced instrument");
    }
    return DatabaseResult<std::int64_t>::success(rows.value().front().front().int64Value);
}

DatabaseResult<ShellAccountingExcelVbaImportManualEntryPersistenceResult>
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::findExistingImportMarker(
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request)
{
    auto rows = connection_.queryRows(
        "SELECT id, COALESCE(new_value_json, '{}') FROM audit_log "
        "WHERE entity_type = ? AND action = ? AND reason = ? "
        "ORDER BY id DESC LIMIT 1;",
        {"SHELL_ACCOUNTING_EXCEL_VBA_IMPORT",
         "PERSIST_MANUAL_ENTRY",
         importAuditReason(request.idempotencyKey)});
    if (!rows) {
        return DatabaseResult<ShellAccountingExcelVbaImportManualEntryPersistenceResult>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    if (rows.value().empty()) {
        ShellAccountingExcelVbaImportManualEntryPersistenceResult result;
        result.status = "NO_DUPLICATE";
        return DatabaseResult<ShellAccountingExcelVbaImportManualEntryPersistenceResult>::success(result);
    }

    const auto& row = rows.value().front();
    ShellAccountingExcelVbaImportManualEntryPersistenceResult result;
    result.accepted = true;
    result.writeImplemented = true;
    result.previewDigest = request.previewDigest;
    result.idempotencyKey = request.idempotencyKey;
    result.requestId = request.requestId;
    result.auditLogId = row[0].int64Value;
    if (auditJsonHasDigest(row[1].text, request.previewDigest)) {
        result.success = true;
        result.databaseWritten = true;
        result.tradeLogWritten = true;
        result.auditLogWritten = true;
        result.idempotentReplay = true;
        result.duplicateImportPrevented = true;
        result.status = "IDEMPOTENT_REPLAY";
    } else {
        result.errorCode = etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD;
        result.idempotencyConflictRejected = true;
        result.status = "IDEMPOTENCY_CONFLICT";
        result.issues.push_back("EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT");
    }
    return DatabaseResult<ShellAccountingExcelVbaImportManualEntryPersistenceResult>::success(result);
}

DatabaseResult<std::int64_t>
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::writeAuditMarker(
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request,
    int tradeLogRowsWritten)
{
    const auto& firstFact = request.tradeFacts.front();
    auto account = findIdByUid("account", firstFact.accountCode, "account");
    if (!account) {
        return DatabaseResult<std::int64_t>::failure(account.error().errorCode, account.error().message);
    }
    auto portfolio = findIdByUid("portfolio", firstFact.portfolioCode, "portfolio");
    if (!portfolio) {
        return DatabaseResult<std::int64_t>::failure(portfolio.error().errorCode, portfolio.error().message);
    }

    AuditLogEntry entry;
    entry.accountId = account.value();
    entry.portfolioId = portfolio.value();
    entry.entityType = "SHELL_ACCOUNTING_EXCEL_VBA_IMPORT";
    entry.entityId = account.value();
    entry.action = "PERSIST_MANUAL_ENTRY";
    entry.oldValueJson = "{}";
    entry.newValueJson = auditNewValueJson(request, tradeLogRowsWritten);
    entry.reason = importAuditReason(request.idempotencyKey);
    entry.operatorName = "dataservice";
    entry.createdAtUtc = isBlank(request.acceptedAtUtc) ? "2026-06-07T00:00:00Z" : request.acceptedAtUtc;

    AuditLogRepository auditLogs(connection_);
    return auditLogs.insertAuditLog(entry);
}

ShellAccountingExcelVbaImportManualEntryPersistenceResult
ShellAccountingExcelVbaImportManualEntryPersistenceRepository::persistAcceptedPreview(
    const ShellAccountingExcelVbaImportManualEntryPersistenceRequest& request)
{
    auto validation = validate(request);
    if (!validation.success && validation.status != "VALIDATION_ACCEPTED") {
        return validation;
    }

    auto existing = findExistingImportMarker(request);
    if (!existing) {
        return failure(
            existing.error().errorCode,
            "IDEMPOTENCY_LOOKUP_FAILED",
            "SAFE_SQLITE_ERROR",
            request);
    }
    if (existing.value().duplicateImportPrevented || existing.value().idempotencyConflictRejected) {
        return existing.value();
    }

    ShellAccountingExcelVbaImportManualEntryPersistenceResult result = validation;
    ShellAccountingManualTransactionRepository manualTransactions(connection_);
    TransactionRunner runner(connection_);
    auto transactionResult = runner.runInTransaction([&]() {
        for (std::size_t index = 0; index < request.tradeFacts.size(); ++index) {
            const auto& fact = request.tradeFacts[index];
            auto account = findIdByUid("account", fact.accountCode, "account");
            if (!account) {
                result = failure(account.error().errorCode, "REFERENCE_VALIDATION_FAILED", "REFERENCE_ROW_MISSING", request);
                return DatabaseResult<bool>::failure(account.error().errorCode, account.error().message);
            }
            auto portfolio = findIdByUid("portfolio", fact.portfolioCode, "portfolio");
            if (!portfolio) {
                result = failure(portfolio.error().errorCode, "REFERENCE_VALIDATION_FAILED", "REFERENCE_ROW_MISSING", request);
                return DatabaseResult<bool>::failure(portfolio.error().errorCode, portfolio.error().message);
            }
            auto instrument = findInstrumentIdByCode(fact.instrumentCode);
            if (!instrument) {
                result = failure(instrument.error().errorCode, "REFERENCE_VALIDATION_FAILED", "REFERENCE_ROW_MISSING", request);
                return DatabaseResult<bool>::failure(instrument.error().errorCode, instrument.error().message);
            }

            auto written = manualTransactions.persistManualTransactionInActiveTransaction(
                commandFromFact(fact, account.value(), portfolio.value(), instrument.value(), request, index));
            if (!written.success || written.duplicate) {
                result = failure(
                    written.errorCode,
                    written.duplicate ? "IDEMPOTENCY_CONFLICT" : written.status,
                    written.duplicate ? "EXCEL_VBA_IMPORT_DUPLICATE_TRADE_FACT_WITHOUT_IMPORT_MARKER"
                                      : "EXCEL_VBA_IMPORT_TRANSACTION_ROLLBACK",
                    request);
                return DatabaseResult<bool>::failure(written.errorCode, written.status);
            }
            ++result.tradeLogRowsWritten;
            result.tradeLogWritten = true;
        }

        auto audit = writeAuditMarker(request, result.tradeLogRowsWritten);
        if (!audit) {
            result = failure(
                audit.error().errorCode,
                "TRANSACTION_ROLLED_BACK",
                "EXCEL_VBA_IMPORT_AUDIT_REQUIRED",
                request);
            return DatabaseResult<bool>::failure(audit.error().errorCode, audit.error().message);
        }
        result.auditLogId = audit.value();
        result.auditLogWritten = true;
        return DatabaseResult<bool>::success(true);
    });

    if (!transactionResult) {
        result.success = false;
        result.databaseWritten = false;
        result.tradeLogWritten = false;
        result.auditLogWritten = false;
        result.transactionCommitted = false;
        result.tradeLogRowsWritten = 0;
        if (result.issues.empty()) {
            result.issues.push_back("EXCEL_VBA_IMPORT_TRANSACTION_ROLLBACK");
        }
        if (result.status == "VALIDATION_ACCEPTED") {
            result.status = "TRANSACTION_ROLLED_BACK";
        }
        return result;
    }

    result.success = true;
    result.databaseWritten = true;
    result.transactionCommitted = true;
    result.status = "OK";
    return result;
}

}  // namespace etfdt::data_access
