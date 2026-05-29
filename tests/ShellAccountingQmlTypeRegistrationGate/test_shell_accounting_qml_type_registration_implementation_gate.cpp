#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <filesystem>
#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

namespace {

bool contains(const std::string& text, const std::string& token)
{
    return text.find(token) != std::string::npos;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs67 = root / "docs" / "67_shell_accounting_qml_type_registration_implementation_gate.md";
    const auto docs68 = root / "docs" / "68_shell_accounting_qml_type_registration_rollback_strategy.md";
    const auto docs65 = root / "docs" / "65_shell_accounting_qml_type_registration_boundary.md";
    const auto docs66 = root / "docs" / "66_shell_accounting_qml_type_registration_test_plan.md";
    for (const auto& path : {docs65, docs66, docs67, docs68}) {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Missing gate document: " << path.generic_string() << '\n';
            return 1;
        }
    }

    const auto gate = readTextFile(docs67);
    for (const auto& token : preGateTokens()) {
        if (!contains(gate, token)) {
            std::cerr << "Implementation gate is missing pre-gate: " << token << '\n';
            return 1;
        }
    }
    if (!contains(gate, "用户明确授权") ||
        !contains(gate, "Registration is not a production QML page") ||
        !contains(gate, "Registration is not real accounting data")) {
        std::cerr << "Implementation gate must require authorization and non-equivalence statements.\n";
        return 1;
    }
    return 0;
}
