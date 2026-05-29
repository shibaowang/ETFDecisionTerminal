#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    if (countTokenInFiles(startupFiles, "ShellAccountingPresenter shellAccountingPresenter") != 1) {
        std::cerr << "ShellAccountingPresenter must be created once in the authorized startup location\n";
        return 1;
    }
    for (const auto& token : {
             "new ShellAccountingPresenter",
             "std::make_unique<ShellAccountingPresenter",
             "std::make_shared<ShellAccountingPresenter",
             "ShellAccountingPresenter {",
         }) {
        if (containsToken(startupFiles, token) || containsToken(productionQmlFiles(root), token)) {
            std::cerr << "unauthorized presenter creation token found\n";
            return 1;
        }
    }
    return 0;
}
