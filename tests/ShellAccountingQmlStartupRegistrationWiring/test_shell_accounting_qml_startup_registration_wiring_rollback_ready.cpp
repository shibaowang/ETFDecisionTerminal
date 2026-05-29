#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs69 = readTextFile(root / "docs" / "69_shell_accounting_qml_startup_registration_gate.md");
    const auto docs70 = readTextFile(root / "docs" / "70_shell_accounting_qml_startup_registration_test_plan.md");
    const auto docs71 = readTextFile(root / "docs" / "71_shell_accounting_qml_startup_registration_wiring.md");
    const auto combined = docs69 + docs70 + docs71;

    for (const auto& token : {
             "rollback",
             "must not call DataServiceClient",
             "must not fake data",
             "must not write database",
             "Unavailable",
             "include",
             "one helper call",
             "minimal CMake wiring",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "startup wiring rollback docs missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
