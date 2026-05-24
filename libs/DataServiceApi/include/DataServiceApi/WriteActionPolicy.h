#pragma once

#include <string>
#include <vector>

namespace etfdt::data_service_api {

[[nodiscard]] bool isWriteActionAllowed(const std::string& actionName);
[[nodiscard]] std::vector<std::string> allowedWriteActions();

}  // namespace etfdt::data_service_api
