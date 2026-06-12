#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (containsForbiddenToken(productionStartupFiles(root), forbiddenRuntimeTokens())) {
        std::cerr << "production startup exposes forbidden accounting runtime access\n";
        return 1;
    }
    const auto pageText = readTextFile(shellAccountingReadOnlyPage(root));
    for (const auto& token : forbiddenRuntimeTokens()) {
        if (shellAccountingQmlContainsForbiddenToken(pageText, token)) {
            std::cerr << "ShellAccounting QML page exposes forbidden runtime token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
