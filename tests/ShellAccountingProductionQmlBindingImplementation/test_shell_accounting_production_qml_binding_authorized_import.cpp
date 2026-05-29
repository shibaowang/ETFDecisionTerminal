#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlFiles = productionQmlFiles(root);
    const auto page = readTextFile(authorizedPagePath(root));
    if (countTokenInFiles(qmlFiles, "import ETFDecisionTerminal.ShellAccounting") != 1 ||
        page.find("import ETFDecisionTerminal.ShellAccounting 1.0") == std::string::npos) {
        std::cerr << "ShellAccounting import must appear exactly once in the authorized page\n";
        return 1;
    }
    return 0;
}
