#include "Diagnostics/DiagnosticSummary.h"

#include <sstream>

namespace etfdt::diagnostics {

DiagnosticSummary buildDiagnosticSummary(const DiagnosticReport& report)
{
    DiagnosticSummary summary;
    summary.title = "Watchdog Manifest Report";
    summary.errorCount = report.errorCount;
    summary.warningCount = report.warningCount;

    std::ostringstream out;
    out << summary.title << '\n';
    out << "Config: " << report.configPath << '\n';
    out << "Version: " << report.version << '\n';
    out << "Services: total=" << report.totalServices << " enabled=" << report.enabledServices
        << " disabled=" << report.disabledServices << '\n';
    out << "Issues: errors=" << report.errorCount << " warnings=" << report.warningCount
        << '\n';

    for (const auto& service : report.services) {
        if (service.enabled && !service.canStart) {
            ++summary.enabledBlockedCount;
        }

        out << '\n' << '[' << service.serviceName << "]\n";
        out << "enabled: " << (service.enabled ? "true" : "false") << '\n';
        out << "supported: " << (service.supported ? "true" : "false") << '\n';
        out << "canStart: " << (service.canStart ? "true" : "false") << '\n';
        out << "executableExists: " << (service.executableExists ? "true" : "false") << '\n';
        out << "workingDirectoryExists: "
            << (service.workingDirectoryExists ? "true" : "false") << '\n';
        out << "socketNamePresent: " << (service.socketNamePresent ? "true" : "false") << '\n';
        out << "healthCheckSupported: "
            << (service.healthCheckSupported ? "true" : "false") << '\n';

        for (const auto& issue : service.issues) {
            out << "issue: " << issue.level << ' ' << issue.code << " - " << issue.message
                << '\n';
        }
    }

    summary.ok = summary.errorCount == 0 && summary.enabledBlockedCount == 0;
    summary.exitCode = summary.ok ? 0 : 1;
    summary.text = out.str();
    return summary;
}

}  // namespace etfdt::diagnostics
