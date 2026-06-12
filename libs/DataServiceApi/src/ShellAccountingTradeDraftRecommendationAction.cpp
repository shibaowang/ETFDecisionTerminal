#include "DataServiceApi/DataServiceActions.h"

#include "DataAccess/ShellAccountingTradeDraftRepository.h"
#include "Protocol/Json.h"
#include "StrategyEngine/StrategyRecommendationEngine.h"

#include <cmath>
#include <cctype>
#include <cstdint>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etfdt::data_service_api {
namespace {

std::string jsonString(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

std::string decodeJsonString(std::string_view value)
{
    std::string decoded;
    decoded.reserve(value.size());
    bool escaping = false;
    for (const char ch : value) {
        if (!escaping) {
            if (ch == '\\') {
                escaping = true;
            } else {
                decoded.push_back(ch);
            }
            continue;
        }

        switch (ch) {
        case '"':
        case '\\':
        case '/':
            decoded.push_back(ch);
            break;
        case 'b':
            decoded.push_back('\b');
            break;
        case 'f':
            decoded.push_back('\f');
            break;
        case 'n':
            decoded.push_back('\n');
            break;
        case 'r':
            decoded.push_back('\r');
            break;
        case 't':
            decoded.push_back('\t');
            break;
        default:
            decoded.push_back(ch);
            break;
        }
        escaping = false;
    }
    return decoded;
}

std::string trim(std::string value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1U);
}

bool isBlank(const std::string& value)
{
    return trim(value).empty();
}

bool isJsonObjectPayloadShape(std::string_view payloadJson)
{
    auto first = payloadJson.begin();
    while (first != payloadJson.end()
           && std::isspace(static_cast<unsigned char>(*first)) != 0) {
        ++first;
    }
    if (first == payloadJson.end() || *first != '{') {
        return false;
    }
    auto last = payloadJson.rbegin();
    while (last != payloadJson.rend()
           && std::isspace(static_cast<unsigned char>(*last)) != 0) {
        ++last;
    }
    return last != payloadJson.rend() && *last == '}';
}

void skipWhitespace(std::string_view value, std::size_t& index)
{
    while (index < value.size()
           && std::isspace(static_cast<unsigned char>(value[index])) != 0) {
        ++index;
    }
}

std::optional<std::string> parseJsonStringAt(
    std::string_view value,
    std::size_t& index)
{
    if (index >= value.size() || value[index] != '"') {
        return std::nullopt;
    }
    ++index;
    std::string raw;
    bool escaping = false;
    while (index < value.size()) {
        const char ch = value[index++];
        if (escaping) {
            raw.push_back('\\');
            raw.push_back(ch);
            escaping = false;
            continue;
        }
        if (ch == '\\') {
            escaping = true;
            continue;
        }
        if (ch == '"') {
            return decodeJsonString(raw);
        }
        raw.push_back(ch);
    }
    return std::nullopt;
}

std::size_t skipJsonValue(std::string_view value, std::size_t index)
{
    skipWhitespace(value, index);
    if (index >= value.size()) {
        return index;
    }
    if (value[index] == '"') {
        ++index;
        bool escaping = false;
        while (index < value.size()) {
            const char ch = value[index++];
            if (escaping) {
                escaping = false;
            } else if (ch == '\\') {
                escaping = true;
            } else if (ch == '"') {
                break;
            }
        }
        return index;
    }
    if (value[index] == '{' || value[index] == '[') {
        const char open = value[index];
        const char close = open == '{' ? '}' : ']';
        int depth = 1;
        ++index;
        bool inString = false;
        bool escaping = false;
        while (index < value.size() && depth > 0) {
            const char ch = value[index++];
            if (inString) {
                if (escaping) {
                    escaping = false;
                } else if (ch == '\\') {
                    escaping = true;
                } else if (ch == '"') {
                    inString = false;
                }
                continue;
            }
            if (ch == '"') {
                inString = true;
            } else if (ch == open) {
                ++depth;
            } else if (ch == close) {
                --depth;
            }
        }
        return index;
    }
    while (index < value.size() && value[index] != ',' && value[index] != '}') {
        ++index;
    }
    return index;
}

std::optional<std::string> topLevelFieldRaw(
    std::string_view payloadJson,
    const std::string& fieldName)
{
    std::size_t index = 0;
    skipWhitespace(payloadJson, index);
    if (index >= payloadJson.size() || payloadJson[index] != '{') {
        return std::nullopt;
    }
    ++index;

    while (index < payloadJson.size()) {
        skipWhitespace(payloadJson, index);
        if (index < payloadJson.size() && payloadJson[index] == ',') {
            ++index;
            skipWhitespace(payloadJson, index);
        }
        if (index >= payloadJson.size() || payloadJson[index] == '}') {
            break;
        }
        auto key = parseJsonStringAt(payloadJson, index);
        if (!key.has_value()) {
            return std::nullopt;
        }
        skipWhitespace(payloadJson, index);
        if (index >= payloadJson.size() || payloadJson[index] != ':') {
            return std::nullopt;
        }
        ++index;
        skipWhitespace(payloadJson, index);
        const auto valueStart = index;
        const auto valueEnd = skipJsonValue(payloadJson, index);
        if (*key == fieldName) {
            return std::string(payloadJson.substr(valueStart, valueEnd - valueStart));
        }
        index = valueEnd;
    }
    return std::nullopt;
}

std::optional<std::string> stringField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    auto raw = topLevelFieldRaw(payloadJson, fieldName);
    if (!raw.has_value()) {
        return std::nullopt;
    }
    std::string_view rawView(*raw);
    std::size_t index = 0;
    skipWhitespace(rawView, index);
    return parseJsonStringAt(rawView, index);
}

std::optional<bool> boolField(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    auto raw = topLevelFieldRaw(payloadJson, fieldName);
    if (!raw.has_value()) {
        return std::nullopt;
    }
    const auto value = trim(*raw);
    if (value == "true") {
        return true;
    }
    if (value == "false") {
        return false;
    }
    return std::nullopt;
}

std::optional<std::int64_t> int64Field(
    const std::string& payloadJson,
    const std::string& fieldName)
{
    if (const auto text = stringField(payloadJson, fieldName); text.has_value()) {
        try {
            std::size_t parsed = 0;
            const auto value = std::stoll(trim(*text), &parsed, 10);
            if (parsed == trim(*text).size()) {
                return value;
            }
        } catch (...) {
            return std::nullopt;
        }
    }

    auto raw = topLevelFieldRaw(payloadJson, fieldName);
    if (!raw.has_value()) {
        return std::nullopt;
    }
    const auto valueText = trim(*raw);
    try {
        std::size_t parsed = 0;
        const auto value = std::stoll(valueText, &parsed, 10);
        if (parsed == valueText.size()) {
            return value;
        }
    } catch (...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::int64_t quantity1e6(std::string value)
{
    value = trim(std::move(value));
    if (value.empty()) {
        return 0;
    }
    try {
        return static_cast<std::int64_t>(std::llround(std::stold(value) * 1000000.0));
    } catch (...) {
        return 0;
    }
}

etfdt::protocol::ProtocolResponse successResponse(
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

etfdt::protocol::ProtocolResponse protocolErrorResponse(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::protocol::ErrorCode code,
    std::string message)
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = context.request.msgId;
    response.traceId = context.request.traceId;
    response.success = false;
    response.errorCode = code;
    response.errorMessage = std::move(message);
    response.payloadJson = "{}";
    return response;
}

template <typename T>
etfdt::protocol::ProtocolResponse errorResponse(
    const etfdt::service_runtime::ActionContext& context,
    const etfdt::data_access::DatabaseResult<T>& result)
{
    return protocolErrorResponse(
        context,
        result.error().errorCode,
        result.error().message.empty() ? "TradeDraft repository operation failed"
                                       : result.error().message);
}

std::string issueCodesJson(const std::vector<std::string>& issueCodes)
{
    std::ostringstream stream;
    stream << '[';
    for (std::size_t index = 0; index < issueCodes.size(); ++index) {
        if (index != 0U) {
            stream << ',';
        }
        stream << jsonString(issueCodes[index]);
    }
    stream << ']';
    return stream.str();
}

std::string issueCodesJson(
    const std::vector<etfdt::strategy_engine::StrategyRecommendationIssue>& issues)
{
    std::vector<std::string> codes;
    codes.reserve(issues.size());
    for (const auto& issue : issues) {
        codes.push_back(issue.code);
    }
    return issueCodesJson(codes);
}

std::string rejectedPayload(
    const etfdt::strategy_engine::TradeDraftConversionResult& conversion)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(kActionAccountingTradeDraftCreateFromRecommendation) << ','
            << "\"task\":\"EPIC-278\","
            << "\"mode\":\"tradedraft-manual-recommendation-flow-full-delivery\","
            << "\"dataServiceWriteActionCreated\":true,"
            << "\"tradeDraftManualRecommendationFlowCreated\":true,"
            << "\"tradeDraftRequiresExplicitUserConfirmation\":true,"
            << "\"explicitUserConfirmationRequired\":true,"
            << "\"recommendationEngineCalled\":true,"
            << "\"recommendationAccepted\":false,"
            << "\"tradeDraftEligible\":false,"
            << "\"conversionNoWrite\":true,"
            << "\"draftWritten\":false,"
            << "\"legWritten\":false,"
            << "\"auditWritten\":false,"
            << "\"transactionCommitted\":false,"
            << "\"duplicateDraft\":false,"
            << "\"idempotencyConflict\":false,"
            << "\"status\":" << jsonString(conversion.status) << ','
            << "\"draftId\":0,"
            << "\"auditLogId\":0,"
            << "\"side\":" << jsonString(conversion.side) << ','
            << "\"instrumentCode\":" << jsonString(conversion.instrumentCode) << ','
            << "\"quantityText\":" << jsonString(conversion.quantityText) << ','
            << "\"amountText\":" << jsonString(conversion.amountText) << ','
            << "\"netCashImpactText\":" << jsonString(conversion.netCashImpactText) << ','
            << "\"issueCodes\":" << issueCodesJson(conversion.issueCodes) << ','
            << "\"tradeDraftIsNotOrder\":true,"
            << "\"multiChannelOtcLegsCreated\":false,"
            << "\"tradeLogRowsWrittenByTradeDraft\":false,"
            << "\"cashAdjustmentRowsWrittenByTradeDraft\":false,"
            << "\"positionChangedByTradeDraft\":false,"
            << "\"cashChangedByTradeDraft\":false,"
            << "\"productionWrite\":false,"
            << "\"sqliteProductionWrite\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"automaticTrading\":false"
            << "}";
    return payload.str();
}

std::string writePayload(
    const etfdt::data_access::ShellAccountingTradeDraftCreateResult& result,
    bool recommendationAccepted,
    bool conversionEligible)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(kActionAccountingTradeDraftCreateFromRecommendation) << ','
            << "\"task\":\"EPIC-278\","
            << "\"mode\":\"tradedraft-manual-recommendation-flow-full-delivery\","
            << "\"dataServiceWriteActionCreated\":true,"
            << "\"tradeDraftManualRecommendationFlowCreated\":true,"
            << "\"tradeDraftRequiresExplicitUserConfirmation\":true,"
            << "\"explicitUserConfirmationRequired\":true,"
            << "\"recommendationEngineCalled\":true,"
            << "\"recommendationAccepted\":" << (recommendationAccepted ? "true" : "false") << ','
            << "\"tradeDraftEligible\":" << (conversionEligible ? "true" : "false") << ','
            << "\"conversionNoWrite\":true,"
            << "\"draftWritten\":" << (result.draftWritten ? "true" : "false") << ','
            << "\"legWritten\":" << (result.legWritten ? "true" : "false") << ','
            << "\"auditWritten\":" << (result.auditWritten ? "true" : "false") << ','
            << "\"transactionCommitted\":" << (result.transactionCommitted ? "true" : "false") << ','
            << "\"duplicateDraft\":" << (result.duplicateDraft ? "true" : "false") << ','
            << "\"idempotencyConflict\":" << (result.idempotencyConflict ? "true" : "false") << ','
            << "\"status\":" << jsonString(result.status) << ','
            << "\"draftId\":" << result.draftId << ','
            << "\"auditLogId\":" << result.auditLogId << ','
            << "\"draftUid\":" << jsonString(result.draftUid) << ','
            << "\"draftSignature\":" << jsonString(result.draftSignature) << ','
            << "\"side\":" << jsonString(result.side) << ','
            << "\"instrumentCode\":" << jsonString(result.instrumentCode) << ','
            << "\"quantityText\":" << jsonString(result.quantityText) << ','
            << "\"amountText\":" << jsonString(result.amountText) << ','
            << "\"netCashImpactText\":" << jsonString(result.netCashImpactText) << ','
            << "\"issueCodes\":" << issueCodesJson(result.issueCodes) << ','
            << "\"tradeDraftIsNotOrder\":true,"
            << "\"multiChannelOtcLegsCreated\":false,"
            << "\"tradeLogRowsWrittenByTradeDraft\":false,"
            << "\"cashAdjustmentRowsWrittenByTradeDraft\":false,"
            << "\"positionChangedByTradeDraft\":false,"
            << "\"cashChangedByTradeDraft\":false,"
            << "\"productionWrite\":false,"
            << "\"sqliteProductionWrite\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"automaticTrading\":false"
            << "}";
    return payload.str();
}

std::string summaryPayload(const etfdt::data_access::ShellAccountingTradeDraftSummary& summary)
{
    std::ostringstream payload;
    payload << "{"
            << "\"action\":" << jsonString(kActionAccountingTradeDraftReadOnlySummary) << ','
            << "\"task\":\"EPIC-278\","
            << "\"mode\":\"tradedraft-readonly-summary\","
            << "\"readOnly\":true,"
            << "\"found\":" << (summary.found ? "true" : "false") << ','
            << "\"draftId\":" << summary.draftId << ','
            << "\"draftUid\":" << jsonString(summary.draftUid) << ','
            << "\"draftSignature\":" << jsonString(summary.draftSignature) << ','
            << "\"status\":" << jsonString(summary.status) << ','
            << "\"side\":" << jsonString(summary.side) << ','
            << "\"accountId\":" << jsonString(summary.accountId) << ','
            << "\"portfolioId\":" << jsonString(summary.portfolioId) << ','
            << "\"instrumentCode\":" << jsonString(summary.instrumentCode) << ','
            << "\"strategyCode\":" << jsonString(summary.strategyCode) << ','
            << "\"quantityText\":" << jsonString(summary.quantityText) << ','
            << "\"amountText\":" << jsonString(summary.amountText) << ','
            << "\"netCashImpactText\":" << jsonString(summary.netCashImpactText) << ','
            << "\"idempotencyKey\":" << jsonString(summary.idempotencyKey) << ','
            << "\"productionWrite\":false,"
            << "\"tradeLogRowsWrittenByTradeDraft\":false,"
            << "\"cashAdjustmentRowsWrittenByTradeDraft\":false,"
            << "\"brokerOrderSubmitted\":false,"
            << "\"networkAccess\":false,"
            << "\"credentialAccess\":false,"
            << "\"endpointAccess\":false,"
            << "\"automaticTrading\":false"
            << "}";
    return payload.str();
}

}  // namespace

etfdt::protocol::ProtocolResponse handleAccountingTradeDraftCreateFromRecommendation(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    const auto& payloadJson = context.request.payloadJson;
    if (!isJsonObjectPayloadShape(payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.tradedraft.create_from_recommendation payload must be a JSON object");
    }

    const auto recommendationPayloadJson =
        stringField(payloadJson, "recommendationPayloadJson").value_or("");
    if (isBlank(recommendationPayloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "TradeDraft recommendation creation requires recommendationPayloadJson");
    }

    etfdt::strategy_engine::StrategyRecommendationInput input;
    std::string errorCode;
    if (!etfdt::strategy_engine::decodeStrategyRecommendationInputJson(
            recommendationPayloadJson,
            input,
            errorCode)) {
        etfdt::strategy_engine::TradeDraftConversionResult rejected;
        rejected.status = "RECOMMENDATION_PARSE_ERROR";
        rejected.issueCodes.push_back(errorCode);
        return successResponse(context, rejectedPayload(rejected));
    }

    const auto recommendation =
        etfdt::strategy_engine::StrategyRecommendationEngine{}.recommendReadOnly(input);

    etfdt::strategy_engine::TradeDraftConversionRequest conversionRequest;
    conversionRequest.recommendation = recommendation;
    conversionRequest.accountId = stringField(payloadJson, "accountId").value_or(input.accountId);
    conversionRequest.portfolioId = stringField(payloadJson, "portfolioId").value_or(input.portfolioId);
    conversionRequest.strategyCode = stringField(payloadJson, "strategyCode").value_or(input.strategyCode);
    conversionRequest.instrumentCode =
        stringField(payloadJson, "instrumentCode").value_or(input.instrumentCode);
    conversionRequest.instrumentType =
        stringField(payloadJson, "instrumentType").value_or(input.instrumentType);
    conversionRequest.tradeSource = stringField(payloadJson, "tradeSource").value_or(input.tradeSource);
    conversionRequest.idempotencyKey = stringField(payloadJson, "idempotencyKey").value_or("");
    conversionRequest.recommendationDigest =
        stringField(payloadJson, "recommendationDigest").value_or("");
    conversionRequest.userNote = stringField(payloadJson, "userNote").value_or("");
    conversionRequest.userConfirmed = boolField(payloadJson, "userConfirmed").value_or(false);

    const auto conversion =
        etfdt::strategy_engine::StrategyRecommendationEngine{}.convertRecommendationToTradeDraft(
            conversionRequest);
    if (!conversion.eligible) {
        return successResponse(context, rejectedPayload(conversion));
    }

    const auto accountId = int64Field(payloadJson, "accountId");
    const auto portfolioId = int64Field(payloadJson, "portfolioId");
    const auto strategyId = int64Field(payloadJson, "strategyId");
    const auto instrumentId = int64Field(payloadJson, "instrumentId");
    if (!accountId.has_value() || !portfolioId.has_value()
        || !strategyId.has_value() || !instrumentId.has_value()) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "TradeDraft recommendation creation requires accountId, portfolioId, strategyId, and instrumentId");
    }

    etfdt::data_access::ShellAccountingTradeDraftCreateRequest request;
    request.accountId = *accountId;
    request.portfolioId = *portfolioId;
    request.strategyId = *strategyId;
    request.strategyCode = conversion.strategyCode;
    request.instrumentId = *instrumentId;
    request.instrumentCode = conversion.instrumentCode;
    request.instrumentType = conversion.instrumentType;
    request.side = conversion.side;
    request.quantity1e6 = quantity1e6(conversion.quantityText);
    request.quantityText = conversion.quantityText;
    request.amountText = conversion.amountText;
    request.priceText = conversion.priceText;
    request.feeEstimateText = conversion.feeEstimateText;
    request.netCashImpactText = conversion.netCashImpactText;
    request.tradeSource = conversion.tradeSource;
    request.reason = "EPIC-278 TradeDraft create from recommendation";
    request.authorizationToken = "EPIC-278_TRADEDRAFT_RECOMMENDATION_WRITE";
    request.createdAtUtc = context.request.timestampUtc;
    request.expiresAtUtc = stringField(payloadJson, "expiresAtUtc").value_or("");
    request.idempotencyKey = conversionRequest.idempotencyKey;
    request.recommendationDigest = conversionRequest.recommendationDigest;
    request.sourceRecommendationAction = conversion.sourceRecommendationAction;
    request.sourceRecommendationReason = conversion.sourceRecommendationReason;
    request.userConfirmationRequired = true;
    request.userConfirmed = conversion.userConfirmed;
    request.baseProtectionPassed = conversion.baseProtectionPassed;
    request.cashLimitApplied = conversion.cashLimitApplied;
    request.otcGenericDraft = conversion.otcGenericDraft;

    etfdt::data_access::ShellAccountingTradeDraftRepository repository(connection);
    auto writeResult = repository.createTradeDraft(request);
    if (!writeResult) {
        return errorResponse(context, writeResult);
    }

    return successResponse(
        context,
        writePayload(writeResult.value(), recommendation.accepted, conversion.eligible));
}

etfdt::protocol::ProtocolResponse handleAccountingTradeDraftReadOnlySummary(
    const etfdt::service_runtime::ActionContext& context,
    etfdt::data_access::SQLiteConnection& connection)
{
    if (!isJsonObjectPayloadShape(context.request.payloadJson)) {
        return protocolErrorResponse(
            context,
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "accounting.tradedraft.readonly_summary payload must be a JSON object");
    }

    const auto idempotencyKey =
        stringField(context.request.payloadJson, "idempotencyKey").value_or("");
    const auto draftId = int64Field(context.request.payloadJson, "draftId").value_or(0);

    etfdt::data_access::ShellAccountingTradeDraftRepository repository(connection);
    auto summary = repository.readTradeDraftSummary(idempotencyKey, draftId);
    if (!summary) {
        return errorResponse(context, summary);
    }
    return successResponse(context, summaryPayload(summary.value()));
}

}  // namespace etfdt::data_service_api
