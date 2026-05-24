#pragma once

#include "Diagnostics/DiagnosticReport.h"

#include <string>

namespace etfdt::diagnostics {

struct DiagnosticSummary final {
    bool ok = false;
    int exitCode = 1;
    std::string title;
    std::string text;
    int errorCount = 0;
    int warningCount = 0;
    int enabledBlockedCount = 0;
};

[[nodiscard]] DiagnosticSummary buildDiagnosticSummary(const DiagnosticReport& report);

}  // namespace etfdt::diagnostics
