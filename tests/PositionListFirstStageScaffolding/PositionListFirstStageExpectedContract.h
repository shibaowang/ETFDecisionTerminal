#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::position_list_first_stage {

[[nodiscard]] std::vector<std::string> allowedFirstStageScope();
[[nodiscard]] std::vector<std::string> explicitlyExcludedFirstStageScope();

[[nodiscard]] bool containsValue(
    const std::vector<std::string>& values,
    const std::string& expected);

}  // namespace etfdt::tests::position_list_first_stage
