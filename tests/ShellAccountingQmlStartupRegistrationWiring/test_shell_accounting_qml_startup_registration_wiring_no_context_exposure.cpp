#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);

    for (const auto& token : {
             "setContextProperty(\"accountingPresenter\"",
             "setContextProperty(\"ShellAccountingPresenter\"",
             "ShellAccountingPresenter ",
             "ShellAccountingPresenter{",
             "ShellAccountingReadOnlyController",
             "ShellAccountingDataServiceAdapter",
             "ShellAccountingDataServiceClientPortAdapter",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "startup must not expose or create ShellAccounting runtime objects\n";
            return 1;
        }
    }
    return 0;
}
