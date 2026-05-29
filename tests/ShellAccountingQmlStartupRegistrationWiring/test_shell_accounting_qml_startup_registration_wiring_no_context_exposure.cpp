#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    if (countTokenInFiles(startupFiles, "setContextProperty(\"accountingPresenter\"") != 1) {
        std::cerr << "startup must expose exactly one authorized accountingPresenter context property\n";
        return 1;
    }
    if (countTokenInFiles(startupFiles, "ShellAccountingPresenter shellAccountingPresenter") != 1) {
        std::cerr << "startup must create exactly one authorized ShellAccountingPresenter\n";
        return 1;
    }

    for (const auto& token : {
             "setContextProperty(\"ShellAccountingPresenter\"",
             "ShellAccountingPresenter{",
             "setContextProperty(\"ShellAccountingReadOnlyController\"",
             "setContextProperty(\"ShellAccountingDataServiceAdapter\"",
             "setContextProperty(\"ShellAccountingDataServiceClientPortAdapter\"",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "startup must not expose unauthorized ShellAccounting runtime objects\n";
            return 1;
        }
    }
    return 0;
}
