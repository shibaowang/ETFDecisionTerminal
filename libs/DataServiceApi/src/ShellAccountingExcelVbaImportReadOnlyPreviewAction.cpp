#include "DataServiceApi/DataServiceActions.h"

#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"
#include "Protocol/Json.h"

#include <cctype>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

struct ImportPayloadDecodeResult {
    ShellAccountingExcelVbaImportPayload payload;
    bool ok = false;
    std::string errorCode;
};

class JsonCursor {
public:
    explicit JsonCursor(std::string_view text)
        : text_(text)
    {
    }

    [[nodiscard]] bool parseRootPayload(ShellAccountingExcelVbaImportPayload& payload)
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
            if (!key.has_value()) {
                return false;
            }
            if (!consume(':')) {
                return false;
            }

            if (*key == "rawUserDataExposed") {
                auto value = parseBool();
                if (!value.has_value()) {
                    return false;
                }
                payload.rawUserDataExposed = *value;
            } else if (*key == "importPayload") {
                if (!parseImportPayloadObject(payload)) {
                    return false;
                }
                foundImportPayload = true;
            } else if (*key == "sheets") {
                if (!parseSheetsArray(payload.sheets)) {
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
        return foundImportPayload && payload.sheets.size() > 0U && position_ == text_.size();
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

            if (*key == "sheetName" || *key == "name") {
                auto value = parseString();
                if (!value.has_value()) {
                    return false;
                }
                if (sheet.sheetName.empty() || *key == "sheetName") {
                    sheet.sheetName = std::move(*value);
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

    [[nodiscard]] std::optional<std::string> parseNumberLiteral()
    {
        skipWhitespace();
        const auto start = position_;
        if (position_ < text_.size() && text_[position_] == '-') {
            ++position_;
        }
        bool digitSeen = false;
        while (position_ < text_.size()
               && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
            digitSeen = true;
            ++position_;
        }
        if (position_ < text_.size() && text_[position_] == '.') {
            ++position_;
            while (position_ < text_.size()
                   && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
                digitSeen = true;
                ++position_;
            }
        }
        if (position_ < text_.size() && (text_[position_] == 'e' || text_[position_] == 'E')) {
            ++position_;
            if (position_ < text_.size() && (text_[position_] == '-' || text_[position_] == '+')) {
                ++position_;
            }
            bool exponentDigitSeen = false;
            while (position_ < text_.size()
                   && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
                exponentDigitSeen = true;
                ++position_;
            }
            digitSeen = digitSeen && exponentDigitSeen;
        }
        if (!digitSeen || start == position_) {
            return std::nullopt;
        }
        return std::string(text_.substr(start, position_ - start));
    }

    [[nodiscard]] std::optional<std::string> parseScalarCell()
    {
        skipWhitespace();
        if (position_ >= text_.size()) {
            return std::nullopt;
        }
        if (text_[position_] == '"') {
            return parseString();
        }
        if (text_.substr(position_, 4U) == "true") {
            position_ += 4U;
            return std::string("true");
        }
        if (text_.substr(position_, 5U) == "false") {
            position_ += 5U;
            return std::string("false");
        }
        if (text_.substr(position_, 4U) == "null") {
            position_ += 4U;
            return std::string();
        }
        return parseNumberLiteral();
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
            if (!parseCellArray(row)) {
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

    [[nodiscard]] bool parseCellArray(std::vector<std::string>& values)
    {
        if (!consume('[')) {
            return false;
        }

        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return true;
            }

            auto value = parseScalarCell();
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
        while (position_ < text_.size() && std::string_view(",]} \t\r\n").find(text_[position_]) == std::string_view::npos) {
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

[[nodiscard]] std::string jsonStringValue(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

[[nodiscard]] ImportPayloadDecodeResult decodeImportPreviewPayload(std::string_view payloadJson)
{
    ImportPayloadDecodeResult result;
    JsonCursor cursor(payloadJson);
    result.ok = cursor.parseRootPayload(result.payload);
    if (!result.ok) {
        result.errorCode = "IMPORT_PAYLOAD_PARSE_ERROR";
    }
    return result;
}

void writeDiagnostic(
    std::ostringstream& stream,
    const ShellAccountingExcelVbaImportDiagnostic& diagnostic)
{
    stream << "{"
           << "\"level\":" << jsonStringValue(diagnostic.level) << ','
           << "\"code\":" << jsonStringValue(diagnostic.code) << ','
           << "\"field\":" << jsonStringValue(diagnostic.field) << ','
           << "\"sheetName\":" << jsonStringValue(diagnostic.sheetName) << ','
           << "\"rowId\":" << jsonStringValue(diagnostic.rowId)
           << "}";
}

[[nodiscard]] std::string diagnosticsJson(
    const std::vector<ShellAccountingExcelVbaImportDiagnostic>& diagnostics)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < diagnostics.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        writeDiagnostic(stream, diagnostics[index]);
    }
    stream << ']';
    return stream.str();
}

[[nodiscard]] std::string diagnosticCodesJson(
    const std::vector<ShellAccountingExcelVbaImportDiagnostic>& diagnostics)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < diagnostics.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << jsonStringValue(diagnostics[index].code);
    }
    stream << ']';
    return stream.str();
}

[[nodiscard]] std::string parseErrorDiagnosticsJson(std::string_view code)
{
    std::ostringstream stream;
    stream << "[{\"level\":\"ERROR\",\"code\":" << jsonStringValue(code)
           << ",\"field\":\"importPayload\",\"sheetName\":\"Workbook\",\"rowId\":\"\"}]";
    return stream.str();
}

[[nodiscard]] etfdt::protocol::ProtocolResponse successResponse(
    const etfdt::service_runtime::ActionContext& context,
    std::string payloadJson)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = true;
    response.payloadJson = std::move(payloadJson);
    return response;
}

[[nodiscard]] std::string previewPayloadJson(
    const ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonStringValue(kActionAccountingExcelVbaImportReadOnlyPreview) << ','
            << "\"task\":\"TASK-258\","
            << "\"mode\":\"readonly-dataservice-import-preview-action-vertical-slice\","
            << "\"dataServiceReadOnlyActionRegistered\":true,"
            << "\"parserBoundaryCalled\":true,"
            << "\"previewActionExecuted\":true,"
            << "\"diagnosticsBuilt\":true,"
            << "\"replayFactSummaryBuilt\":true,"
            << "\"accountingEngineCalled\":false,"
            << "\"productionRuntimeIntegrationImplemented\":false,"
            << "\"accepted\":" << (parseResult.accepted ? "true" : "false") << ','
            << "\"diagnostics\":" << diagnosticsJson(parseResult.diagnostics) << ','
            << "\"diagnosticCodes\":" << diagnosticCodesJson(parseResult.diagnostics) << ','
            << "\"replayFactSummary\":{"
            << "\"tradeFactCount\":" << parseResult.tradeFacts.size() << ','
            << "\"cashFactCount\":" << parseResult.cashFacts.size() << ','
            << "\"marketPriceFactCount\":" << parseResult.marketPriceFacts.size() << ','
            << "\"fxRateFactCount\":" << parseResult.fxRateFacts.size() << ','
            << "\"configFactCount\":" << parseResult.configFactCount << ','
            << "\"strategyFactCount\":" << parseResult.strategyFactCount << ','
            << "\"skippedRows\":" << parseResult.skippedRows << ','
            << "\"sensitiveHeadersIgnored\":" << parseResult.sensitiveHeadersIgnored
            << "},"
            << "\"parserReadOnly\":" << (parseResult.parserReadOnly ? "true" : "false") << ','
            << "\"parserNoSqliteAccess\":" << (parseResult.parserNoSqliteAccess ? "true" : "false") << ','
            << "\"parserNoWrite\":" << (parseResult.parserNoWrite ? "true" : "false") << ','
            << "\"productionWrite\":false,"
            << "\"sqliteProductionWrite\":false,"
            << "\"auditWritten\":false,"
            << "\"ledgerWritten\":false,"
            << "\"snapshotWritten\":false,"
            << "\"tradeLogWritten\":false,"
            << "\"readModelPersistentWrite\":false,"
            << "\"tradeDraftGenerated\":false,"
            << "\"strategyExecuted\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"automaticTrading\":false,"
            << "\"rawUserDataExposed\":" << (parseResult.rawUserDataExposed ? "true" : "false")
            << "}";
    return payload.str();
}

[[nodiscard]] std::string parseErrorPayloadJson(std::string_view code)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonStringValue(kActionAccountingExcelVbaImportReadOnlyPreview) << ','
            << "\"task\":\"TASK-258\","
            << "\"mode\":\"readonly-dataservice-import-preview-action-vertical-slice\","
            << "\"dataServiceReadOnlyActionRegistered\":true,"
            << "\"parserBoundaryCalled\":false,"
            << "\"previewActionExecuted\":true,"
            << "\"diagnosticsBuilt\":true,"
            << "\"replayFactSummaryBuilt\":true,"
            << "\"accountingEngineCalled\":false,"
            << "\"productionRuntimeIntegrationImplemented\":false,"
            << "\"accepted\":false,"
            << "\"diagnostics\":" << parseErrorDiagnosticsJson(code) << ','
            << "\"diagnosticCodes\":[" << jsonStringValue(code) << "],"
            << "\"replayFactSummary\":{\"tradeFactCount\":0,\"cashFactCount\":0,\"marketPriceFactCount\":0,\"fxRateFactCount\":0},"
            << "\"parserReadOnly\":true,"
            << "\"parserNoSqliteAccess\":true,"
            << "\"parserNoWrite\":true,"
            << "\"productionWrite\":false,"
            << "\"sqliteProductionWrite\":false,"
            << "\"auditWritten\":false,"
            << "\"ledgerWritten\":false,"
            << "\"snapshotWritten\":false,"
            << "\"tradeLogWritten\":false,"
            << "\"readModelPersistentWrite\":false,"
            << "\"tradeDraftGenerated\":false,"
            << "\"strategyExecuted\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"automaticTrading\":false,"
            << "\"rawUserDataExposed\":false"
            << "}";
    return payload.str();
}

}  // namespace

etfdt::protocol::ProtocolResponse handleAccountingExcelVbaImportReadOnlyPreview(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    (void)connection;

    const auto decoded = decodeImportPreviewPayload(context.request.payloadJson);
    if (!decoded.ok) {
        return successResponse(context, parseErrorPayloadJson(decoded.errorCode));
    }

    const auto parseResult = parseShellAccountingExcelVbaImportReadOnly(decoded.payload);
    return successResponse(context, previewPayloadJson(parseResult));
}

}  // namespace etfdt::data_service_api
