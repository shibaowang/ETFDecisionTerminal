#include "Protocol/Protocol.h"

#include <iostream>
#include <string>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectContains(
    const std::string& haystack,
    std::string_view needle,
    std::string_view message)
{
    if (haystack.find(needle) == std::string::npos) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " missing " << needle << '\n';
    }
}

etfdt::protocol::MessageEnvelope validEnvelope()
{
    using etfdt::protocol::ServiceName;

    etfdt::protocol::MessageEnvelope envelope;
    envelope.msgId = "msg-001";
    envelope.traceId = "trace-001";
    envelope.from = ServiceName::ETFDecisionShell;
    envelope.to = ServiceName::ETFDataService;
    envelope.action = "Ping";
    envelope.timestampUtc = "2026-05-24T10:30:00Z";
    envelope.payloadJson = "{}";
    return envelope;
}

etfdt::protocol::ProtocolResponse validSuccessResponse()
{
    etfdt::protocol::ProtocolResponse response;
    response.msgId = "msg-001";
    response.traceId = "trace-001";
    response.success = true;
    response.payloadJson = "{}";
    return response;
}

void testServiceName()
{
    using etfdt::protocol::ServiceName;
    using etfdt::protocol::fromString;
    using etfdt::protocol::toString;

    expectEqual(
        toString(ServiceName::ETFDataService),
        "ETFDataService",
        "ServiceName ETFDataService string");

    auto strategyService = fromString<ServiceName>("ETFStrategyService");
    expectTrue(strategyService.hasValue(), "ServiceName parses ETFStrategyService");
    if (strategyService.hasValue()) {
        expectTrue(
            strategyService.value() == ServiceName::ETFStrategyService,
            "ServiceName parsed enum");
    }

    auto unknown = fromString<ServiceName>("Unknown");
    expectTrue(!unknown.hasValue(), "ServiceName rejects Unknown");
}

void testErrorCode()
{
    using etfdt::core::LogLevel;
    using etfdt::protocol::ErrorCode;
    using etfdt::protocol::defaultSeverity;
    using etfdt::protocol::errorCategory;
    using etfdt::protocol::fromString;
    using etfdt::protocol::numericCode;
    using etfdt::protocol::toString;

    expectEqual(
        toString(ErrorCode::E1001_INVALID_JSON),
        "E1001_INVALID_JSON",
        "ErrorCode string");
    expectEqual(numericCode(ErrorCode::E1001_INVALID_JSON), 1001, "ErrorCode numeric");
    expectEqual(
        errorCategory(ErrorCode::E2001_DATABASE_OPEN_FAILED),
        "DATABASE",
        "ErrorCode category");

    const auto auditSeverity = defaultSeverity(ErrorCode::E3002_CASH_AUDIT_FAILED);
    expectTrue(
        auditSeverity == LogLevel::Error || auditSeverity == LogLevel::Critical,
        "Cash audit severity is at least error");

    auto serviceUnavailable = fromString<ErrorCode>("E9001_SERVICE_UNAVAILABLE");
    expectTrue(serviceUnavailable.hasValue(), "ErrorCode parses E9001_SERVICE_UNAVAILABLE");
    if (serviceUnavailable.hasValue()) {
        expectTrue(
            serviceUnavailable.value() == ErrorCode::E9001_SERVICE_UNAVAILABLE,
            "ErrorCode parsed enum");
    }

    auto badCode = fromString<ErrorCode>("BAD_CODE");
    expectTrue(!badCode.hasValue(), "ErrorCode rejects BAD_CODE");
}

void testMessageEnvelope()
{
    using etfdt::protocol::MessageEnvelope;
    using etfdt::protocol::kProtocolVersion;
    using etfdt::protocol::toJsonString;
    using etfdt::protocol::validateMessageEnvelope;

    MessageEnvelope defaults;
    expectEqual(defaults.protocolVersion, kProtocolVersion, "MessageEnvelope default version");
    expectEqual(defaults.payloadJson, "{}", "MessageEnvelope default payload");

    auto missingMsgId = validEnvelope();
    missingMsgId.msgId.clear();
    expectTrue(!validateMessageEnvelope(missingMsgId).valid, "MessageEnvelope requires msgId");

    auto missingTraceId = validEnvelope();
    missingTraceId.traceId.clear();
    expectTrue(
        !validateMessageEnvelope(missingTraceId).valid,
        "MessageEnvelope requires traceId");

    auto missingAction = validEnvelope();
    missingAction.action.clear();
    expectTrue(!validateMessageEnvelope(missingAction).valid, "MessageEnvelope requires action");

    auto valid = validEnvelope();
    expectTrue(validateMessageEnvelope(valid).valid, "MessageEnvelope accepts JSON object payload");

    auto arrayPayload = validEnvelope();
    arrayPayload.payloadJson = "[]";
    expectTrue(
        validateMessageEnvelope(arrayPayload).valid,
        "MessageEnvelope accepts JSON array payload");

    auto badPayload = validEnvelope();
    badPayload.payloadJson = "not-json";
    expectTrue(!validateMessageEnvelope(badPayload).valid, "MessageEnvelope rejects bad payload");

    const std::string json = toJsonString(valid);
    expectContains(json, "\"protocolVersion\": \"1.0\"", "MessageEnvelope JSON version");
    expectContains(json, "\"msgId\": \"msg-001\"", "MessageEnvelope JSON msgId");
    expectContains(json, "\"traceId\": \"trace-001\"", "MessageEnvelope JSON traceId");
    expectContains(json, "\"from\": \"ETFDecisionShell\"", "MessageEnvelope JSON from");
    expectContains(json, "\"to\": \"ETFDataService\"", "MessageEnvelope JSON to");
    expectContains(json, "\"action\": \"Ping\"", "MessageEnvelope JSON action");
    expectContains(
        json,
        "\"timestamp\": \"2026-05-24T10:30:00Z\"",
        "MessageEnvelope JSON timestamp");
    expectContains(json, "\"payload\": {}", "MessageEnvelope JSON raw payload");
}

void testProtocolResponse()
{
    using etfdt::protocol::ErrorCode;
    using etfdt::protocol::toJsonString;
    using etfdt::protocol::validateProtocolResponse;

    auto success = validSuccessResponse();
    expectTrue(validateProtocolResponse(success).valid, "Success response validates");

    auto failedMissingErrorCode = validSuccessResponse();
    failedMissingErrorCode.success = false;
    failedMissingErrorCode.errorMessage = "failed";
    expectTrue(
        !validateProtocolResponse(failedMissingErrorCode).valid,
        "Failed response requires errorCode");

    auto failedMissingMessage = validSuccessResponse();
    failedMissingMessage.success = false;
    failedMissingMessage.errorCode = ErrorCode::E1001_INVALID_JSON;
    expectTrue(
        !validateProtocolResponse(failedMissingMessage).valid,
        "Failed response requires errorMessage");

    auto errorResponse = validSuccessResponse();
    errorResponse.success = false;
    errorResponse.errorCode = ErrorCode::E1001_INVALID_JSON;
    errorResponse.errorMessage = "invalid json";
    expectTrue(validateProtocolResponse(errorResponse).valid, "Error response validates");

    const std::string json = toJsonString(errorResponse);
    expectContains(json, "\"success\": false", "Response JSON success false");
    expectContains(
        json,
        "\"errorCode\": \"E1001_INVALID_JSON\"",
        "Response JSON errorCode");
    expectContains(json, "\"errorMessage\": \"invalid json\"", "Response JSON errorMessage");
}

void testJsonHelpers()
{
    using etfdt::protocol::escapeJsonString;
    using etfdt::protocol::isLikelyJsonObjectOrArray;

    expectEqual(
        escapeJsonString("a\"b\\c\n"),
        "a\\\"b\\\\c\\n",
        "JSON string escaping");
    expectTrue(isLikelyJsonObjectOrArray("  {} "), "JSON object likely valid");
    expectTrue(isLikelyJsonObjectOrArray("[]"), "JSON array likely valid");
    expectTrue(!isLikelyJsonObjectOrArray("\"text\""), "JSON string is not envelope payload");
}

}  // namespace

int main()
{
    testServiceName();
    testErrorCode();
    testMessageEnvelope();
    testProtocolResponse();
    testJsonHelpers();

    if (gFailures != 0) {
        std::cerr << gFailures << " Protocol test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Protocol tests passed." << '\n';
    return 0;
}
