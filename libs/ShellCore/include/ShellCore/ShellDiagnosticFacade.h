#pragma once

#include "Diagnostics/DiagnosticReport.h"
#include "Diagnostics/DiagnosticSummary.h"
#include "ShellCore/ShellDiagnosticTools.h"
#include "ShellCore/ShellDiagnosticViewModel.h"

#include <filesystem>
#include <string>

namespace etfdt::shell {

class ShellDiagnosticFacade final {
public:
    [[nodiscard]] ShellDiagnosticLoadResult loadFromWatchdogReportFile(const std::string& path);
    [[nodiscard]] ShellDiagnosticLoadResult loadFromWatchdogReportJson(const std::string& json);
    [[nodiscard]] ShellDiagnosticLoadResult loadViewModelFromFile(
        const std::string& path,
        const ShellDiagnosticFilter& filter,
        const ShellDiagnosticSort& sort);
    [[nodiscard]] const std::string& lastError() const;

    [[nodiscard]] static ShellDiagnosticViewModel buildViewModel(
        const diagnostics::DiagnosticReport& report,
        const diagnostics::DiagnosticSummary& summary);
    [[nodiscard]] static ShellDiagnosticViewModel buildViewModel(
        const diagnostics::DiagnosticReport& report,
        const diagnostics::DiagnosticSummary& summary,
        const ShellDiagnosticFilter& filter,
        const ShellDiagnosticSort& sort);
    [[nodiscard]] static ShellDiagnosticViewModel applyFilter(
        const ShellDiagnosticViewModel& viewModel,
        const ShellDiagnosticFilter& filter);
    [[nodiscard]] static ShellDiagnosticViewModel applySort(
        const ShellDiagnosticViewModel& viewModel,
        const ShellDiagnosticSort& sort);
    [[nodiscard]] static ShellDiagnosticAggregate aggregate(
        const ShellDiagnosticViewModel& viewModel);

    [[nodiscard]] ShellDiagnosticRefreshState checkReportFile(const std::string& path) const;
    [[nodiscard]] ShellDiagnosticRefreshResult refreshIfChanged(const std::string& path);

private:
    void updateRefreshState(
        const std::string& path,
        bool loadSucceeded,
        const std::string& errorMessage,
        const ShellDiagnosticViewModel& viewModel);

    std::string lastError_;
    std::string lastReportPath_;
    std::filesystem::file_time_type lastReportWriteTime_{};
    bool hasLastReportWriteTime_ = false;
    bool lastLoadSucceeded_ = false;
    ShellDiagnosticViewModel lastViewModel_;
};

}  // namespace etfdt::shell
