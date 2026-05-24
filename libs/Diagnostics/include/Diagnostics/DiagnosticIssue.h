#pragma once

#include <string>

namespace etfdt::diagnostics {

struct DiagnosticIssue final {
    std::string level;
    std::string code;
    std::string message;
};

}  // namespace etfdt::diagnostics
