#pragma once

#include "Diagnostics/DiagnosticReport.h"
#include "Diagnostics/DiagnosticSummary.h"

#include <string>

namespace etfdt::diagnostics {

struct DiagnosticValidationResult final {
    bool ok = false;
    std::string errorMessage;
};

struct WatchdogReportParseResult final {
    bool ok = false;
    DiagnosticReport report;
    std::string errorMessage;
};

class WatchdogReportParser final {
public:
    [[nodiscard]] static WatchdogReportParseResult parseFromFile(const std::string& path);
    [[nodiscard]] static WatchdogReportParseResult parseFromJsonString(const std::string& json);
    [[nodiscard]] static DiagnosticValidationResult validateReport(const DiagnosticReport& report);
    [[nodiscard]] static DiagnosticSummary summarize(const DiagnosticReport& report);
};

}  // namespace etfdt::diagnostics
