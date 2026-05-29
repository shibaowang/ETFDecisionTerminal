#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    const auto qmlFiles = productionQmlFiles(root);

    if (containsToken(startupFiles, "registerShellAccountingQmlTypes")) {
        std::cerr << "production startup must not call registerShellAccountingQmlTypes in TASK-128\n";
        return 1;
    }
    if (containsToken(qmlFiles, "import ETFDecisionTerminal.ShellAccounting") ||
        containsToken(qmlFiles, "accountingPresenter") ||
        containsToken(qmlFiles, "ShellAccountingPresenter")) {
        std::cerr << "production QML must remain unbound in TASK-128\n";
        return 1;
    }
    return 0;
}
