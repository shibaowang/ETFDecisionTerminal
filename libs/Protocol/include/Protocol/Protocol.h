#pragma once

#include <string>
#include <string_view>

namespace etfdt::protocol {

constexpr std::string_view kProtocolVersion = "0.1-skeleton";

struct MessageEnvelope final {
    std::string sourceService;
    std::string targetService;
    std::string messageType;
    std::string payloadJson;
};

}  // namespace etfdt::protocol
