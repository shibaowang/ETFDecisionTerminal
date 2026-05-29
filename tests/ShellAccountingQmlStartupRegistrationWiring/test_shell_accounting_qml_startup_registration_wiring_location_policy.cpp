#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs71 =
        readTextFile(root / "docs" / "71_shell_accounting_qml_startup_registration_wiring.md");

    for (const auto& token : {
             "centralized",
             "easy to scan",
             "near existing app QML type registration",
             "no scattered startup registration",
             "no plugin hidden registration",
             "no test-only registration being used as production registration",
             "rollback",
         }) {
        if (docs71.find(token) == std::string::npos) {
            std::cerr << "docs/71 missing location policy token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
