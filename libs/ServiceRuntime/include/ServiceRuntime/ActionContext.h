#pragma once

#include "Protocol/Protocol.h"

#include <map>
#include <optional>
#include <string>

namespace etfdt::service_runtime {

struct ActionContext final {
    etfdt::protocol::MessageEnvelope request;
    etfdt::protocol::ServiceName serviceName = etfdt::protocol::ServiceName::ETFDataService;
    std::string receivedAtUtc;
    std::string traceId;
    std::optional<std::string> remotePeerId;
    std::map<std::string, std::string> metadata;
};

[[nodiscard]] std::string currentUtcTimestamp();

[[nodiscard]] ActionContext makeActionContext(
    const etfdt::protocol::MessageEnvelope& request,
    etfdt::protocol::ServiceName serviceName,
    std::optional<std::string> remotePeerId = std::nullopt,
    std::map<std::string, std::string> metadata = {});

}  // namespace etfdt::service_runtime
