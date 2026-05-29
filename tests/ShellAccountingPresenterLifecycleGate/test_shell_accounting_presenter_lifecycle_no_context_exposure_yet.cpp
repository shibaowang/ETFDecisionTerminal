#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    if (countToken(startupFiles, "setContextProperty(\"accountingPresenter\"") != 1) {
        std::cerr << "production startup must expose exactly one authorized accountingPresenter context property\n";
        return 1;
    }
    for (const auto& token : {
             "setContextProperty(\"ShellAccountingPresenter\"",
             "setInitialProperties",
             "setContextProperty(\"ShellAccountingReadOnlyController\"",
             "setContextProperty(\"ShellAccountingDataServiceAdapter\"",
             "setContextProperty(\"ShellAccountingDataServiceClientPortAdapter\"",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "production startup has exposed presenter or accounting services\n";
            return 1;
        }
    }
    return 0;
}
