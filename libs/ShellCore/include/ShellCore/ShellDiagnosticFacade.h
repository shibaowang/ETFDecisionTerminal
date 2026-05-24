#pragma once

#include "Diagnostics/DiagnosticReport.h"
#include "Diagnostics/DiagnosticSummary.h"
#include "ShellCore/ShellDiagnosticViewModel.h"

#include <string>

namespace etfdt::shell {

class ShellDiagnosticFacade final {
public:
    [[nodiscard]] ShellDiagnosticLoadResult loadFromWatchdogReportFile(const std::string& path);
    [[nodiscard]] ShellDiagnosticLoadResult loadFromWatchdogReportJson(const std::string& json);
    [[nodiscard]] const std::string& lastError() const;

    [[nodiscard]] static ShellDiagnosticViewModel buildViewModel(
        const diagnostics::DiagnosticReport& report,
        const diagnostics::DiagnosticSummary& summary);

private:
    std::string lastError_;
};

}  // namespace etfdt::shell
