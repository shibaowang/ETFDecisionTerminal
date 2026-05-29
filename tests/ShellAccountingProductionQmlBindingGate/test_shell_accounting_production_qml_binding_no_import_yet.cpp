#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto qmlFiles = productionQmlFiles(sourceRoot(argc, argv));
    if (containsToken(qmlFiles, "import ETFDecisionTerminal.ShellAccounting")) {
        std::cerr << "production QML must not import ShellAccounting before an authorized binding task\n";
        return 1;
    }
    return 0;
}
