#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    if (countTokenInFiles(startupFiles, "setContextProperty(\"accountingPresenter\"") != 1) {
        std::cerr << "accountingPresenter context property must be exposed exactly once\n";
        return 1;
    }
    for (const auto& token : {
             "setContextProperty(\"DataServiceClient\"",
             "setContextProperty(\"AccountingEngine\"",
             "setContextProperty(\"SQLite\"",
             "setContextProperty(\"DataAccess\"",
             "setContextProperty(\"ShellAccountingDataServiceAdapter\"",
             "setContextProperty(\"ShellAccountingDataServiceClientPortAdapter\"",
             "setContextProperty(\"ShellAccountingReadOnlyController\"",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "forbidden context property exposure found\n";
            return 1;
        }
    }
    return 0;
}
