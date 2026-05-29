#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    for (const auto& token : {
             "new ShellAccountingPresenter",
             "std::make_unique<ShellAccountingPresenter",
             "std::make_shared<ShellAccountingPresenter",
             "ShellAccountingPresenter accountingPresenter",
             "ShellAccountingPresenter* accountingPresenter",
         }) {
        if (containsToken(startupFiles, token)) {
            std::cerr << "production startup has created ShellAccountingPresenter\n";
            return 1;
        }
    }

    const auto pageText = readTextFile(shellAccountingReadOnlyPage(root));
    if (pageText.find("property ShellAccountingPresenter accountingPresenter: null") == std::string::npos) {
        std::cerr << "authorized QML page lost nullable presenter declaration\n";
        return 1;
    }
    if (pageText.find("ShellAccountingPresenter {") != std::string::npos ||
        pageText.find("ShellAccountingPresenter(") != std::string::npos) {
        std::cerr << "production QML must not create ShellAccountingPresenter\n";
        return 1;
    }
    return 0;
}
