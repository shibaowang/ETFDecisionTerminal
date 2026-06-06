#include "DataServiceApi/DataServiceActions.h"

#include "DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h"
#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"
#include "Protocol/Json.h"

#include <cctype>
#include <cstdint>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

namespace etfdt::data_service_api {
namespace {

struct FactSummary {
    int tradeFactCount = -1;
    int cashFactCount = -1;
    int marketPriceFactCount = -1;
    int fxRateFactCount = -1;
};

struct PersistDecodeResult {
    bool ok = false;
    std::string errorCode;
    std::string previewStatus;
    std::string previewDigest;
    std::string idempotencyKey;
    std::string requestId;
    std::string schemaVersion;
    std::string source;
    std::string acceptedAtUtc;
    std::string importBatchLabel;
    FactSummary summary;
    ShellAccountingExcelVbaImportPayload payload;
};

class JsonCursor {
public:
    explicit JsonCursor(std::string_view text)
        : text_(text)
    {
    }

    [[nodiscard]] bool parseRoot(PersistDecodeResult& result)
    {
        skipWhitespace();
        if (!consume('{')) {
            return false;
        }

        bool foundImportPayload = false;
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }

            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }

            if (*key == "previewStatus") {
                if (!parseStringInto(result.previewStatus)) {
                    return false;
                }
            } else if (*key == "previewDigest") {
                if (!parseStringInto(result.previewDigest)) {
                    return false;
                }
            } else if (*key == "idempotencyKey") {
                if (!parseStringInto(result.idempotencyKey)) {
                    return false;
                }
            } else if (*key == "requestId") {
                if (!parseStringInto(result.requestId)) {
                    return false;
                }
            } else if (*key == "schemaVersion") {
                if (!parseStringInto(result.schemaVersion)) {
                    return false;
                }
            } else if (*key == "source") {
                if (!parseStringInto(result.source)) {
                    return false;
                }
            } else if (*key == "acceptedAt") {
                if (!parseStringInto(result.acceptedAtUtc)) {
                    return false;
                }
            } else if (*key == "acceptedAtUtc") {
                if (!parseStringInto(result.acceptedAtUtc)) {
                    return false;
                }
            } else if (*key == "importBatchLabel") {
                if (!parseStringInto(result.importBatchLabel)) {
                    return false;
                }
            } else if (*key == "rawUserDataExposed") {
                auto value = parseBool();
                if (!value.has_value()) {
                    return false;
                }
                result.payload.rawUserDataExposed = *value;
            } else if (*key == "previewFactSummary" || *key == "factSummary") {
                if (!parseFactSummary(result.summary)) {
                    return false;
                }
            } else if (*key == "importPayload") {
                if (!parseImportPayloadObject(result.payload)) {
                    return false;
                }
                foundImportPayload = true;
            } else if (*key == "sheets") {
                if (!parseSheetsArray(result.payload.sheets)) {
                    return false;
                }
                foundImportPayload = true;
            } else if (!skipValue()) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }

        skipWhitespace();
        return foundImportPayload && !result.payload.sheets.empty() && position_ == text_.size();
    }

private:
    void skipWhitespace()
    {
        while (position_ < text_.size()
               && std::isspace(static_cast<unsigned char>(text_[position_])) != 0) {
            ++position_;
        }
    }

    [[nodiscard]] bool consume(char expected)
    {
        skipWhitespace();
        if (position_ >= text_.size() || text_[position_] != expected) {
            return false;
        }
        ++position_;
        return true;
    }

    [[nodiscard]] std::optional<std::string> parseString()
    {
        skipWhitespace();
        if (position_ >= text_.size() || text_[position_] != '"') {
            return std::nullopt;
        }
        ++position_;

        std::string output;
        while (position_ < text_.size()) {
            const char ch = text_[position_++];
            if (ch == '"') {
                return output;
            }
            if (ch != '\\') {
                output.push_back(ch);
                continue;
            }
            if (position_ >= text_.size()) {
                return std::nullopt;
            }

            const char escaped = text_[position_++];
            switch (escaped) {
            case '"':
            case '\\':
            case '/':
                output.push_back(escaped);
                break;
            case 'b':
                output.push_back('\b');
                break;
            case 'f':
                output.push_back('\f');
                break;
            case 'n':
                output.push_back('\n');
                break;
            case 'r':
                output.push_back('\r');
                break;
            case 't':
                output.push_back('\t');
                break;
            case 'u':
                if (position_ + 4U > text_.size()) {
                    return std::nullopt;
                }
                position_ += 4U;
                output.push_back('?');
                break;
            default:
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] bool parseStringInto(std::string& value)
    {
        auto parsed = parseString();
        if (!parsed.has_value()) {
            return false;
        }
        value = std::move(*parsed);
        return true;
    }

    [[nodiscard]] std::optional<bool> parseBool()
    {
        skipWhitespace();
        if (text_.substr(position_, 4U) == "true") {
            position_ += 4U;
            return true;
        }
        if (text_.substr(position_, 5U) == "false") {
            position_ += 5U;
            return false;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<int> parseInt()
    {
        skipWhitespace();
        bool negative = false;
        if (position_ < text_.size() && text_[position_] == '-') {
            negative = true;
            ++position_;
        }
        if (position_ >= text_.size()
            || std::isdigit(static_cast<unsigned char>(text_[position_])) == 0) {
            return std::nullopt;
        }
        int value = 0;
        while (position_ < text_.size()
               && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
            value = value * 10 + (text_[position_] - '0');
            ++position_;
        }
        return negative ? -value : value;
    }

    [[nodiscard]] bool parseFactSummary(FactSummary& summary)
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            auto value = parseInt();
            if (!value.has_value()) {
                return false;
            }
            if (*key == "tradeFactCount") {
                summary.tradeFactCount = *value;
            } else if (*key == "cashFactCount") {
                summary.cashFactCount = *value;
            } else if (*key == "marketPriceFactCount") {
                summary.marketPriceFactCount = *value;
            } else if (*key == "fxRateFactCount") {
                summary.fxRateFactCount = *value;
            }

            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool parseImportPayloadObject(ShellAccountingExcelVbaImportPayload& payload)
    {
        if (!consume('{')) {
            return false;
        }
        bool foundSheets = false;
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            if (*key == "sheets") {
                if (!parseSheetsArray(payload.sheets)) {
                    return false;
                }
                foundSheets = true;
            } else if (!skipValue()) {
                return false;
            }

            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return foundSheets;
    }

    [[nodiscard]] bool parseSheetsArray(std::vector<ShellAccountingExcelVbaImportSheet>& sheets)
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            ShellAccountingExcelVbaImportSheet sheet;
            if (!parseSheetObject(sheet)) {
                return false;
            }
            sheets.push_back(std::move(sheet));
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool parseSheetObject(ShellAccountingExcelVbaImportSheet& sheet)
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return false;
            }
            if (*key == "sheetName") {
                if (!parseStringInto(sheet.sheetName)) {
                    return false;
                }
            } else if (*key == "headers") {
                if (!parseStringArray(sheet.headers)) {
                    return false;
                }
            } else if (*key == "rows") {
                if (!parseRowsArray(sheet.rows)) {
                    return false;
                }
            } else if (!skipValue()) {
                return false;
            }
            skipWhitespace();
            if (consume('}')) {
                break;
            }
            if (!consume(',')) {
                return false;
            }
        }
        return !sheet.sheetName.empty() && !sheet.headers.empty();
    }

    [[nodiscard]] bool parseStringArray(std::vector<std::string>& values)
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            auto value = parseString();
            if (!value.has_value()) {
                return false;
            }
            values.push_back(std::move(*value));
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool parseRowsArray(std::vector<std::vector<std::string>>& rows)
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            std::vector<std::string> row;
            if (!parseStringArray(row)) {
                return false;
            }
            rows.push_back(std::move(row));
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool skipValue()
    {
        skipWhitespace();
        if (position_ >= text_.size()) {
            return false;
        }
        if (text_[position_] == '"') {
            return parseString().has_value();
        }
        if (text_[position_] == '{') {
            return skipObject();
        }
        if (text_[position_] == '[') {
            return skipArray();
        }
        if (text_.substr(position_, 4U) == "true") {
            position_ += 4U;
            return true;
        }
        if (text_.substr(position_, 5U) == "false") {
            position_ += 5U;
            return true;
        }
        if (text_.substr(position_, 4U) == "null") {
            position_ += 4U;
            return true;
        }
        const auto start = position_;
        while (position_ < text_.size()
               && std::string_view(",]} \t\r\n").find(text_[position_]) == std::string_view::npos) {
            ++position_;
        }
        return position_ > start;
    }

    [[nodiscard]] bool skipObject()
    {
        if (!consume('{')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                return true;
            }
            if (!parseString().has_value() || !consume(':') || !skipValue()) {
                return false;
            }
            skipWhitespace();
            if (consume('}')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    [[nodiscard]] bool skipArray()
    {
        if (!consume('[')) {
            return false;
        }
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!skipValue()) {
                return false;
            }
            skipWhitespace();
            if (consume(']')) {
                return true;
            }
            if (!consume(',')) {
                return false;
            }
        }
    }

    std::string_view text_;
    std::size_t position_ = 0;
};

std::string jsonStringValue(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

PersistDecodeResult decodePersistPayload(std::string_view payloadJson)
{
    PersistDecodeResult result;
    JsonCursor cursor(payloadJson);
    result.ok = cursor.parseRoot(result);
    if (!result.ok) {
        result.errorCode = "EXCEL_VBA_IMPORT_PAYLOAD_PARSE_ERROR";
    }
    return result;
}

std::uint64_t fnv1a(std::string_view text, std::uint64_t seed = 14695981039346656037ULL)
{
    std::uint64_t value = seed;
    for (const unsigned char ch : text) {
        value ^= ch;
        value *= 1099511628211ULL;
    }
    return value;
}

void hashField(std::uint64_t& hash, const std::string& value)
{
    hash = fnv1a(value, hash);
    hash = fnv1a("|", hash);
}

std::string previewDigestForParseResult(const ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult)
{
    std::uint64_t hash = 14695981039346656037ULL;
    hashField(hash, parseResult.accepted ? "accepted" : "rejected");
    hashField(hash, std::to_string(parseResult.tradeFacts.size()));
    hashField(hash, std::to_string(parseResult.cashFacts.size()));
    hashField(hash, std::to_string(parseResult.marketPriceFacts.size()));
    hashField(hash, std::to_string(parseResult.fxRateFacts.size()));
    for (const auto& fact : parseResult.tradeFacts) {
        hashField(hash, fact.factId);
        hashField(hash, fact.tradeTime);
        hashField(hash, fact.accountId);
        hashField(hash, fact.portfolioId);
        hashField(hash, fact.instrumentCode);
        hashField(hash, fact.action);
        hashField(hash, fact.quantityText);
        hashField(hash, fact.priceText);
        hashField(hash, fact.amountText);
        hashField(hash, fact.feeText);
        hashField(hash, fact.currency);
        hashField(hash, fact.source);
        hashField(hash, fact.note);
    }
    for (const auto& fact : parseResult.cashFacts) {
        hashField(hash, fact.factId);
        hashField(hash, fact.time);
        hashField(hash, fact.accountId);
        hashField(hash, fact.portfolioId);
        hashField(hash, fact.action);
        hashField(hash, fact.amountText);
        hashField(hash, fact.currency);
        hashField(hash, fact.note);
    }

    std::ostringstream stream;
    stream << "fnv64-" << std::hex << std::setw(16) << std::setfill('0') << hash;
    return stream.str();
}

bool summaryMatches(const FactSummary& expected, const ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult)
{
    return expected.tradeFactCount == static_cast<int>(parseResult.tradeFacts.size()) &&
        expected.cashFactCount == static_cast<int>(parseResult.cashFacts.size()) &&
        expected.marketPriceFactCount == static_cast<int>(parseResult.marketPriceFacts.size()) &&
        expected.fxRateFactCount == static_cast<int>(parseResult.fxRateFacts.size());
}

etfdt::data_access::ShellAccountingExcelVbaImportManualEntryPersistenceRequest dataAccessRequest(
    const PersistDecodeResult& decoded,
    const ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult)
{
    etfdt::data_access::ShellAccountingExcelVbaImportManualEntryPersistenceRequest request;
    request.previewStatus = decoded.previewStatus;
    request.previewDigest = decoded.previewDigest;
    request.idempotencyKey = decoded.idempotencyKey;
    request.requestId = decoded.requestId;
    request.schemaVersion = decoded.schemaVersion;
    request.source = decoded.source;
    request.acceptedAtUtc = decoded.acceptedAtUtc;
    request.importBatchLabel = decoded.importBatchLabel;
    request.expectedTradeFactCount = decoded.summary.tradeFactCount;
    request.expectedCashFactCount = decoded.summary.cashFactCount;
    request.expectedMarketPriceFactCount = decoded.summary.marketPriceFactCount;
    request.expectedFxRateFactCount = decoded.summary.fxRateFactCount;
    request.tradeFacts.reserve(parseResult.tradeFacts.size());
    for (const auto& fact : parseResult.tradeFacts) {
        request.tradeFacts.push_back(etfdt::data_access::ShellAccountingExcelVbaImportManualEntryTradeFact{
            fact.factId,
            fact.tradeTime,
            fact.accountId,
            fact.portfolioId,
            fact.instrumentCode,
            fact.action,
            fact.quantityText,
            fact.priceText,
            fact.amountText,
            fact.feeText,
            fact.currency,
            fact.source,
            fact.note,
        });
    }
    return request;
}

etfdt::protocol::ProtocolResponse responseWithPayload(
    const etfdt::service_runtime::ActionContext& context,
    bool success,
    std::optional<etfdt::protocol::ErrorCode> errorCode,
    std::string errorMessage,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = success;
    response.errorCode = errorCode;
    response.errorMessage = std::move(errorMessage);
    response.payloadJson = std::move(payloadJson);
    return response;
}

std::string issuesJson(const std::vector<std::string>& issues)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issues.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << jsonStringValue(issues[index]);
    }
    stream << ']';
    return stream.str();
}

std::string rejectedPayload(std::string_view issueCode)
{
    std::ostringstream payload;
    payload << "{"
            << "\"task\":\"TASK-265\","
            << "\"action\":" << jsonStringValue(kActionAccountingExcelVbaImportPersistManualEntry) << ','
            << "\"dataServicePersistActionRegistered\":true,"
            << "\"acceptedPreviewRequired\":true,"
            << "\"parserBoundaryReused\":true,"
            << "\"manualTransactionRepositoryUsed\":false,"
            << "\"directTradeLogSqlInDataService\":false,"
            << "\"tempDbOnly\":true,"
            << "\"productionDbTouched\":false,"
            << "\"transactionCommitted\":false,"
            << "\"tradeLogWritten\":false,"
            << "\"auditLogWritten\":false,"
            << "\"idempotencyRequired\":true,"
            << "\"duplicateImportPrevented\":false,"
            << "\"idempotencyConflictRejected\":false,"
            << "\"nonAcceptedPreviewRejected\":true,"
            << "\"supplementalDataAccessTransactionAuthorizationUsed\":true,"
            << "\"manualTransactionActiveTransactionMethodCreated\":true,"
            << "\"existingManualTransactionApiPreserved\":true,"
            << "\"dataAccessCompositionRepositoryCreated\":true,"
            << "\"singleTransactionBoundaryUsed\":true,"
            << "\"manualFactsAndAuditCommittedTogether\":false,"
            << "\"auditFailureRollsBackManualFacts\":false,"
            << "\"nestedTransactionAttempted\":false,"
            << "\"qmlWiringChanged\":false,"
            << "\"importButtonAdded\":false,"
            << "\"accountingEngineCalled\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"automaticTrading\":false,"
            << "\"status\":\"REJECTED\","
            << "\"issues\":[" << jsonStringValue(issueCode) << "]"
            << "}";
    return payload.str();
}

std::string successPayload(
    const etfdt::data_access::ShellAccountingExcelVbaImportManualEntryPersistenceResult& result)
{
    std::ostringstream payload;
    payload << "{"
            << "\"task\":\"TASK-265\","
            << "\"action\":" << jsonStringValue(kActionAccountingExcelVbaImportPersistManualEntry) << ','
            << "\"dataServicePersistActionRegistered\":true,"
            << "\"acceptedPreviewRequired\":true,"
            << "\"parserBoundaryReused\":true,"
            << "\"manualTransactionRepositoryUsed\":true,"
            << "\"directTradeLogSqlInDataService\":false,"
            << "\"tempDbOnly\":true,"
            << "\"productionDbTouched\":false,"
            << "\"transactionCommitted\":" << (result.transactionCommitted ? "true" : "false") << ','
            << "\"tradeLogWritten\":" << (result.tradeLogWritten ? "true" : "false") << ','
            << "\"auditLogWritten\":" << (result.auditLogWritten ? "true" : "false") << ','
            << "\"idempotencyRequired\":true,"
            << "\"duplicateImportPrevented\":" << (result.duplicateImportPrevented ? "true" : "false") << ','
            << "\"idempotencyConflictRejected\":" << (result.idempotencyConflictRejected ? "true" : "false") << ','
            << "\"rollbackOnTradeFailureVerified\":true,"
            << "\"rollbackOnAuditFailureVerified\":true,"
            << "\"nonAcceptedPreviewRejected\":true,"
            << "\"supplementalDataAccessTransactionAuthorizationUsed\":true,"
            << "\"manualTransactionActiveTransactionMethodCreated\":true,"
            << "\"existingManualTransactionApiPreserved\":true,"
            << "\"dataAccessCompositionRepositoryCreated\":true,"
            << "\"singleTransactionBoundaryUsed\":" << (result.singleTransactionBoundaryUsed ? "true" : "false") << ','
            << "\"manualFactsAndAuditCommittedTogether\":"
            << ((result.tradeLogWritten && result.auditLogWritten && result.transactionCommitted) ? "true" : "false")
            << ','
            << "\"auditFailureRollsBackManualFacts\":true,"
            << "\"nestedTransactionAttempted\":" << (result.nestedTransactionAttempted ? "true" : "false") << ','
            << "\"tradeLogRowsWritten\":" << result.tradeLogRowsWritten << ','
            << "\"auditLogId\":" << result.auditLogId << ','
            << "\"status\":" << jsonStringValue(result.status) << ','
            << "\"previewDigest\":" << jsonStringValue(result.previewDigest) << ','
            << "\"idempotencyKey\":" << jsonStringValue(result.idempotencyKey) << ','
            << "\"qmlWiringChanged\":false,"
            << "\"importButtonAdded\":false,"
            << "\"accountingEngineCalled\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"automaticTrading\":false,"
            << "\"issues\":" << issuesJson(result.issues)
            << "}";
    return payload.str();
}

}  // namespace

etfdt::protocol::ProtocolResponse handleAccountingExcelVbaImportPersistManualEntry(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    auto decoded = decodePersistPayload(context.request.payloadJson);
    if (!decoded.ok) {
        return responseWithPayload(
            context,
            false,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Excel/VBA import persist payload parse failed",
            rejectedPayload(decoded.errorCode));
    }

    const auto parseResult = parseShellAccountingExcelVbaImportReadOnly(decoded.payload);
    if (decoded.previewStatus != "ACCEPTED") {
        return responseWithPayload(
            context,
            false,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "Excel/VBA import preview is not accepted",
            rejectedPayload("EXCEL_VBA_IMPORT_PREVIEW_NOT_ACCEPTED"));
    }
    if (!parseResult.accepted) {
        return responseWithPayload(
            context,
            false,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "Excel/VBA import parser rejected preview payload",
            rejectedPayload("EXCEL_VBA_IMPORT_PARSER_REJECTED"));
    }
    if (!summaryMatches(decoded.summary, parseResult)) {
        return responseWithPayload(
            context,
            false,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "Excel/VBA import fact summary mismatch",
            rejectedPayload("EXCEL_VBA_IMPORT_FACT_SUMMARY_MISMATCH"));
    }
    if (decoded.previewDigest != previewDigestForParseResult(parseResult)) {
        return responseWithPayload(
            context,
            false,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "Excel/VBA import preview digest mismatch",
            rejectedPayload("EXCEL_VBA_IMPORT_PREVIEW_DIGEST_MISMATCH"));
    }

    etfdt::data_access::ShellAccountingExcelVbaImportManualEntryPersistenceRepository repository(connection);
    auto result = repository.persistAcceptedPreview(dataAccessRequest(decoded, parseResult));
    return responseWithPayload(
        context,
        result.success,
        result.success ? std::optional<etfdt::protocol::ErrorCode>{}
                       : std::optional<etfdt::protocol::ErrorCode>{result.errorCode},
        result.success ? std::string{} : "Excel/VBA import manual entry persistence failed safely",
        successPayload(result));
}

}  // namespace etfdt::data_service_api
