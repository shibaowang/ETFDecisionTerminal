#pragma once

#include "Protocol/ErrorCode.h"
#include "Protocol/ProtocolVersion.h"
#include "Protocol/ServiceName.h"

#include <optional>
#include <string>

namespace etfdt::protocol {

struct ProtocolValidationError final {
    ErrorCode code = ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR;
    std::string message;
};

struct ValidationResult final {
    bool valid = true;
    std::optional<ProtocolValidationError> error;

    [[nodiscard]] static ValidationResult success();
    [[nodiscard]] static ValidationResult failure(ErrorCode code, std::string message);
};

struct MessageEnvelope final {
    std::string protocolVersion = std::string(kProtocolVersion);
    std::string msgId;
    std::string traceId;
    ServiceName from = ServiceName::ETFDecisionShell;
    ServiceName to = ServiceName::ETFDataService;
    std::string action;
    std::string timestampUtc;
    std::string payloadJson = "{}";
};

struct ProtocolResponse final {
    std::string protocolVersion = std::string(kProtocolVersion);
    std::string msgId;
    std::string traceId;
    bool success = true;
    std::optional<ErrorCode> errorCode;
    std::string errorMessage;
    std::string payloadJson = "{}";
};

[[nodiscard]] ValidationResult validateMessageEnvelope(const MessageEnvelope& envelope);
[[nodiscard]] ValidationResult validateProtocolResponse(const ProtocolResponse& response);

[[nodiscard]] std::string toJsonString(const MessageEnvelope& envelope);
[[nodiscard]] std::string toJsonString(const ProtocolResponse& response);

}  // namespace etfdt::protocol
