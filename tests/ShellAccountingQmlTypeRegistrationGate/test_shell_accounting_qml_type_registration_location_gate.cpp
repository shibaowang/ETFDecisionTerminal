#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = readTextFile(root / "docs" / "67_shell_accounting_qml_type_registration_implementation_gate.md");
    for (const auto& rule : registrationLocationRules()) {
        if (gate.find(rule) == std::string::npos) {
            std::cerr << "Implementation gate is missing registration location rule: "
                      << rule << '\n';
            return 1;
        }
    }
    return 0;
}
