#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = readTextFile(root / "docs" / "67_shell_accounting_qml_type_registration_implementation_gate.md");
    for (const auto& token : forbiddenTypeTokens()) {
        if (gate.find(token) == std::string::npos) {
            std::cerr << "Implementation gate missing forbidden type: " << token << '\n';
            return 1;
        }
    }
    for (const auto& token : forbiddenMethodPropertyTokens()) {
        if (gate.find(token) == std::string::npos) {
            std::cerr << "Implementation gate missing forbidden method/property: " << token << '\n';
            return 1;
        }
    }
    return 0;
}
