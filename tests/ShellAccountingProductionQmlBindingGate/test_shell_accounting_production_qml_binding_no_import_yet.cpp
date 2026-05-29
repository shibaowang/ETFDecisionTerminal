#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlFiles = productionQmlFiles(root);
    const auto importToken = std::string("import ETFDecisionTerminal.ShellAccounting 1.0");

    if (countTokenInFiles(qmlFiles, "import ETFDecisionTerminal.ShellAccounting") != 1) {
        std::cerr << "production QML must contain exactly one authorized ShellAccounting import\n";
        return 1;
    }
    if (readTextFile(authorizedShellAccountingQmlBindingFile(root)).find(importToken) == std::string::npos) {
        std::cerr << "authorized ShellAccounting import must stay in ShellAccountingReadOnlyPage.qml\n";
        return 1;
    }
    return 0;
}
