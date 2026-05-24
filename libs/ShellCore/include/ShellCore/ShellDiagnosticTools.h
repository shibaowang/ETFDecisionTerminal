#pragma once

#include "ShellCore/ShellDiagnosticViewModel.h"

#include <string>

namespace etfdt::shell {

enum class ShellDiagnosticSortKey {
    ServiceName,
    Severity,
    CanStart,
    Enabled,
    IssueCount,
};

struct ShellDiagnosticFilter final {
    bool includeDisabled = true;
    bool onlyEnabled = false;
    bool onlyBlocked = false;
    bool onlyWithIssues = false;
    std::string minimumSeverity = "OK";
    std::string searchText;
};

struct ShellDiagnosticSort final {
    ShellDiagnosticSortKey sortKey = ShellDiagnosticSortKey::ServiceName;
    bool ascending = true;
};

struct ShellDiagnosticAggregate final {
    int total = 0;
    int enabled = 0;
    int disabled = 0;
    int ok = 0;
    int warning = 0;
    int error = 0;
    int blocked = 0;
    int withIssues = 0;
};

struct ShellDiagnosticRefreshState final {
    std::string reportPath;
    bool exists = false;
    std::string lastModifiedUtc;
    bool changedSinceLastLoad = false;
    bool lastLoadSucceeded = false;
    std::string lastError;
};

struct ShellDiagnosticRefreshResult final {
    bool ok = false;
    bool reloaded = false;
    ShellDiagnosticRefreshState refreshState;
    ShellDiagnosticViewModel viewModel;
    std::string errorMessage;
};

}  // namespace etfdt::shell
