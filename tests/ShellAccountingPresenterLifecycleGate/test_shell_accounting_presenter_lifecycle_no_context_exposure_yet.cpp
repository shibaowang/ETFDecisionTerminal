#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    for (const auto& token : {
             "setContextProperty(\"accountingPresenter\"",
             "setContextProperty(\"ShellAccountingPresenter\"",
             "setInitialProperties",
             "ShellAccountingReadOnlyController ",
             "ShellAccountingDataServiceAdapter ",
             "ShellAccountingDataServiceClientPortAdapter ",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "production startup has exposed presenter or accounting services\n";
            return 1;
        }
    }
    return 0;
}
