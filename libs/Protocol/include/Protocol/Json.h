#pragma once

#include <string>
#include <string_view>

namespace etfdt::protocol {

[[nodiscard]] std::string escapeJsonString(std::string_view value);
[[nodiscard]] bool isLikelyJsonObjectOrArray(std::string_view value);

}  // namespace etfdt::protocol
