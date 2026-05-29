#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);

    if (countTokenInFiles(startupFiles, "registerShellAccountingQmlTypes") != 1) {
        std::cerr << "ShellAccounting startup wiring must remain a single helper call\n";
        return 1;
    }
    for (const auto& token : {
             "qmlRegisterType<ShellAccounting",
             "qmlRegisterUncreatableType<ShellAccounting",
             "qmlRegisterSingletonType<ShellAccounting",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "startup must not directly register ShellAccounting types\n";
            return 1;
        }
    }
    return 0;
}
