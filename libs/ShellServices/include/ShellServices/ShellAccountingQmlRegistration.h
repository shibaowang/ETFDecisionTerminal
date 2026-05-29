#pragma once

namespace etfdt::shell_services {

struct ShellAccountingQmlRegistrationOptions final {
    const char* uri = "ETFDecisionTerminal.ShellAccounting";
    int majorVersion = 1;
    int minorVersion = 0;
};

[[nodiscard]] bool registerShellAccountingQmlTypes(
    const ShellAccountingQmlRegistrationOptions& options = {});

}  // namespace etfdt::shell_services
