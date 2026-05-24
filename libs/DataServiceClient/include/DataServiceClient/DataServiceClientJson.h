#pragma once

#include "DataServiceClient/DataServiceClientTypes.h"

#include <string>

namespace etfdt::data_service_client {

[[nodiscard]] DataServiceClientResult<etfdt::protocol::ProtocolResponse> parseProtocolResponseJson(
    const std::string& json);

[[nodiscard]] std::string auditAppendPayloadJson(const AuditAppendRequest& request);

}  // namespace etfdt::data_service_client
