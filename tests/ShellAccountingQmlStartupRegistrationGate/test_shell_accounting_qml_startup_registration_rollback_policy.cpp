#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs68 = readTextFile(root / "docs" / "68_shell_accounting_qml_type_registration_rollback_strategy.md");
    const auto docs69 = readTextFile(root / "docs" / "69_shell_accounting_qml_startup_registration_gate.md");
    const auto docs70 = readTextFile(root / "docs" / "70_shell_accounting_qml_startup_registration_test_plan.md");

    if (docs68.empty()) {
        std::cerr << "docs/68 rollback strategy must exist\n";
        return 1;
    }
    for (const auto& token : rollbackPolicyTokens()) {
        if (docs69.find(token) == std::string::npos && docs70.find(token) == std::string::npos &&
            docs68.find(token) == std::string::npos) {
            std::cerr << "rollback policy missing `" << token << "`\n";
            return 1;
        }
    }
    if (docs69.find("must not call `DataServiceClient`") == std::string::npos ||
        docs69.find("must not fake data") == std::string::npos ||
        docs69.find("must not write any database table") == std::string::npos ||
        docs69.find("Unavailable") == std::string::npos ||
        docs69.find("no accounting") == std::string::npos) {
        std::cerr << "startup rollback fallback rules are incomplete\n";
        return 1;
    }
    return 0;
}
