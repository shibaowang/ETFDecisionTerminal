#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs71 =
        readTextFile(root / "docs" / "71_shell_accounting_qml_startup_registration_wiring.md");

    if (countTokenInFiles(productionStartupFiles(root), "registerShellAccountingQmlTypes") != 1) {
        std::cerr << "production startup must call registerShellAccountingQmlTypes exactly once\n";
        return 1;
    }
    if (docs71.find("TASK-129") == std::string::npos ||
        docs71.find("not QML page binding") == std::string::npos ||
        docs71.find("not real accounting action") == std::string::npos) {
        std::cerr << "docs/71 must record TASK-129 wiring and its non-binding boundary\n";
        return 1;
    }
    return 0;
}
