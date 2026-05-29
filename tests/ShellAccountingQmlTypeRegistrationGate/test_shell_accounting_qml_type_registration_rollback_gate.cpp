#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <filesystem>
#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto rollbackPath = root / "docs" / "68_shell_accounting_qml_type_registration_rollback_strategy.md";
    if (!std::filesystem::exists(rollbackPath)) {
        std::cerr << "Missing rollback strategy document.\n";
        return 1;
    }
    const auto rollback = readTextFile(rollbackPath);
    for (const auto& rule : rollbackRules()) {
        if (rollback.find(rule) == std::string::npos) {
            std::cerr << "Rollback strategy missing rule: " << rule << '\n';
            return 1;
        }
    }
    for (const auto& trigger : {
             "QML static gate failure",
             "Smoke test failure",
             "Unexpected write path",
             "Issue hidden or Unavailable misclassified",
         }) {
        if (rollback.find(trigger) == std::string::npos) {
            std::cerr << "Rollback strategy missing trigger: " << trigger << '\n';
            return 1;
        }
    }
    return 0;
}
