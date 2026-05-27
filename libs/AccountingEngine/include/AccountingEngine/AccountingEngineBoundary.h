#pragma once

#include <string>
#include <vector>

namespace etfdt::accounting {

struct AccountingEngineBoundary {
    std::vector<std::string> allowedDependencies;
    std::vector<std::string> forbiddenDependencies;
    std::vector<std::string> forbiddenWrites;
    std::vector<std::string> forbiddenRuntimeActions;
    std::vector<std::string> testOnlySourcesNotAllowed;
};

[[nodiscard]] AccountingEngineBoundary accountingEngineBoundary();

} // namespace etfdt::accounting
