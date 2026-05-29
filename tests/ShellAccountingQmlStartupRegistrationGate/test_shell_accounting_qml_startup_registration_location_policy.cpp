#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs69 = readTextFile(root / "docs" / "69_shell_accounting_qml_startup_registration_gate.md");
    for (const auto& token : locationPolicyTokens()) {
        if (docs69.find(token) == std::string::npos) {
            std::cerr << "docs/69 missing startup location policy `" << token << "`\n";
            return 1;
        }
    }
    if (docs69.find("registration from QML") == std::string::npos ||
        docs69.find("hidden plugin") == std::string::npos) {
        std::cerr << "startup location policy must ban QML dynamic and hidden plugin registration\n";
        return 1;
    }
    return 0;
}
