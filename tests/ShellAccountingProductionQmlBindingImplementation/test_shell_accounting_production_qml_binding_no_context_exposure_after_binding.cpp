#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    if (countTokenInFiles(startupFiles, "setContextProperty(\"accountingPresenter\"") != 1) {
        std::cerr << "production startup must expose exactly one authorized accountingPresenter\n";
        return 1;
    }
    if (countTokenInFiles(startupFiles, "ShellAccountingPresenter shellAccountingPresenter") != 1) {
        std::cerr << "production startup must create exactly one authorized ShellAccountingPresenter\n";
        return 1;
    }
    for (const auto& token : {
             "setContextProperty(\"ShellAccountingPresenter\"",
             "ShellAccountingPresenter{",
             "setContextProperty(\"ShellAccountingReadOnlyController\"",
             "setContextProperty(\"ShellAccountingDataServiceAdapter\"",
             "setContextProperty(\"ShellAccountingDataServiceClientPortAdapter\"",
         }) {
        if (containsForbiddenToken(startupFiles, {token})) {
            std::cerr << "production startup must not expose unauthorized ShellAccounting context objects\n";
            return 1;
        }
    }
    return 0;
}
