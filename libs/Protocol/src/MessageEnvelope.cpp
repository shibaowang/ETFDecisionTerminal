#include "Protocol/MessageEnvelope.h"

#include "Protocol/Json.h"

#include <cctype>
#include <sstream>
#include <utility>

namespace etfdt::protocol {
namespace {

bool isBlank(std::string_view value)
{
    for (const char ch : value) {
        if (std::isspace(static_cast<unsigned char>(ch)) == 0) {
            return false;
        }
    }
    return true;
}

bool hasValidActionFormat(std::string_view action)
{
    if (isBlank(action)) {
        return false;
    }

    for (const char ch : action) {
        const auto uch = static_cast<unsigned char>(ch);
        if (std::isalnum(uch) != 0 || ch == '_' || ch == '-' || ch == '.' || ch == ':') {
            continue;
        }
        return false;
    }
    return true;
}

void appendJsonStringField(
    std::ostringstream& stream,
    std::string_view key,
    std::string_view value,
    bool trailingComma)
{
    stream << '"' << key << "\": \"" << escapeJsonString(value) << '"';
    if (trailingComma) {
        stream << ',';
    }
    stream << '\n';
}

ValidationResult validateCommonFields(
    std::string_view protocolVersion,
    std::string_view msgId,
    std::string_view traceId,
    std::string_view payloadJson)
{
    if (protocolVersion != kProtocolVersion) {
        return ValidationResult::failure(
            ErrorCode::E1003_UNSUPPORTED_PROTOCOL_VERSION,
            "Unsupported protocol version");
    }
    if (isBlank(msgId)) {
        return ValidationResult::failure(
            ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "msgId is required");
    }
    if (isBlank(traceId)) {
        return ValidationResult::failure(
            ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "traceId is required");
    }
    if (!isLikelyJsonObjectOrArray(payloadJson)) {
        return ValidationResult::failure(
            ErrorCode::E1001_INVALID_JSON,
            "payloadJson must be a JSON object or array fragment");
    }
    return ValidationResult::success();
}

}  // namespace

ValidationResult ValidationResult::success()
{
    return {};
}

ValidationResult ValidationResult::failure(ErrorCode code, std::string message)
{
    ValidationResult result;
    result.valid = false;
    result.error = ProtocolValidationError{code, std::move(message)};
    return result;
}

ValidationResult validateMessageEnvelope(const MessageEnvelope& envelope)
{
    auto common = validateCommonFields(
        envelope.protocolVersion,
        envelope.msgId,
        envelope.traceId,
        envelope.payloadJson);
    if (!common.valid) {
        return common;
    }

    if (toString(envelope.from).empty() || toString(envelope.to).empty()) {
        return ValidationResult::failure(
            ErrorCode::E1005_INVALID_SERVICE_NAME,
            "from and to must be valid service names");
    }
    if (!hasValidActionFormat(envelope.action)) {
        return ValidationResult::failure(
            ErrorCode::E1004_INVALID_ACTION,
            "action is required and may contain only letters, digits, _, -, . or :");
    }
    if (isBlank(envelope.timestampUtc)) {
        return ValidationResult::failure(
            ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "timestampUtc is required");
    }

    return ValidationResult::success();
}

ValidationResult validateProtocolResponse(const ProtocolResponse& response)
{
    auto common = validateCommonFields(
        response.protocolVersion,
        response.msgId,
        response.traceId,
        response.payloadJson);
    if (!common.valid) {
        return common;
    }

    if (response.success) {
        if (response.errorCode.has_value() || !response.errorMessage.empty()) {
            return ValidationResult::failure(
                ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
                "successful response must not contain errorCode or errorMessage");
        }
    }
    else {
        if (!response.errorCode.has_value()) {
            return ValidationResult::failure(
                ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "failed response requires errorCode");
        }
        if (toString(*response.errorCode).empty()) {
            return ValidationResult::failure(
                ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
                "failed response contains unknown errorCode");
        }
        if (isBlank(response.errorMessage)) {
            return ValidationResult::failure(
                ErrorCode::E1002_MISSING_REQUIRED_FIELD,
                "failed response requires errorMessage");
        }
    }

    return ValidationResult::success();
}

std::string toJsonString(const MessageEnvelope& envelope)
{
    std::ostringstream stream;
    stream << "{\n";
    appendJsonStringField(stream, "protocolVersion", envelope.protocolVersion, true);
    appendJsonStringField(stream, "msgId", envelope.msgId, true);
    appendJsonStringField(stream, "traceId", envelope.traceId, true);
    appendJsonStringField(stream, "from", toString(envelope.from), true);
    appendJsonStringField(stream, "to", toString(envelope.to), true);
    appendJsonStringField(stream, "action", envelope.action, true);
    appendJsonStringField(stream, "timestamp", envelope.timestampUtc, true);
    stream << "\"payload\": " << envelope.payloadJson << '\n';
    stream << '}';
    return stream.str();
}

std::string toJsonString(const ProtocolResponse& response)
{
    std::ostringstream stream;
    stream << "{\n";
    appendJsonStringField(stream, "protocolVersion", response.protocolVersion, true);
    appendJsonStringField(stream, "msgId", response.msgId, true);
    appendJsonStringField(stream, "traceId", response.traceId, true);
    stream << "\"success\": " << (response.success ? "true" : "false") << ",\n";
    appendJsonStringField(
        stream,
        "errorCode",
        response.errorCode.has_value() ? toString(*response.errorCode) : "",
        true);
    appendJsonStringField(stream, "errorMessage", response.errorMessage, true);
    stream << "\"payload\": " << response.payloadJson << '\n';
    stream << '}';
    return stream.str();
}

}  // namespace etfdt::protocol
