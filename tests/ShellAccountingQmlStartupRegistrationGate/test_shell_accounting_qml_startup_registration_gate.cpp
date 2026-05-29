#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs69 = readTextFile(root / "docs" / "69_shell_accounting_qml_startup_registration_gate.md");
    const auto docs70 = readTextFile(root / "docs" / "70_shell_accounting_qml_startup_registration_test_plan.md");

    for (const auto& token : requiredPreGateTokens()) {
        if (docs69.find(token) == std::string::npos) {
            std::cerr << "docs/69 missing pre-gate token " << token << '\n';
            return 1;
        }
    }
    if (docs69.find("explicit user authorization") == std::string::npos ||
        docs70.find("User explicitly authorized startup wiring") == std::string::npos) {
        std::cerr << "startup wiring gate must require explicit user authorization\n";
        return 1;
    }
    if (docs69.find("not production QML page binding") == std::string::npos ||
        docs69.find("not real accounting action") == std::string::npos) {
        std::cerr << "startup wiring boundary must distinguish page binding and real accounting actions\n";
        return 1;
    }
    return 0;
}
