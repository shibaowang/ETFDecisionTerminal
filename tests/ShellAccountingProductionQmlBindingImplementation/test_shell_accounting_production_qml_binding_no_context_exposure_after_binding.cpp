#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

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
        if (containsForbiddenToken(startupFiles, {token})) {
            std::cerr << "production startup must not expose ShellAccounting context objects\n";
            return 1;
        }
    }
    return 0;
}
