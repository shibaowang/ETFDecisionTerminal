#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto gate = readTextFile(root / "docs" / "67_shell_accounting_qml_type_registration_implementation_gate.md");
    for (const auto& type : firstStageAllowedTypes()) {
        if (gate.find(type) == std::string::npos) {
            std::cerr << "Implementation gate missing first-stage allowed type: " << type << '\n';
            return 1;
        }
    }
    if (gate.find("read-only properties") == std::string::npos ||
        gate.find("read-only model roles") == std::string::npos) {
        std::cerr << "Allowed type gate must restrict exposure to read-only properties and roles.\n";
        return 1;
    }

    const auto boundary = readTextFile(root / "docs" / "65_shell_accounting_qml_type_registration_boundary.md");
    for (const auto& futureType : futureButNotFirstStageTypes()) {
        if (boundary.find(futureType) == std::string::npos ||
            boundary.find("not part of the first-stage") == std::string::npos) {
            std::cerr << "Future type must remain denied in first-stage docs: " << futureType << '\n';
            return 1;
        }
    }
    return 0;
}
