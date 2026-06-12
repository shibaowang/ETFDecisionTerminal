#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    const auto shellAccountingPageText = readTextFile(shellAccountingReadOnlyPage(root));
    for (const auto& token : forbiddenRuntimeTokens()) {
        if (containsToken(startupFiles, token) ||
            shellAccountingQmlContainsForbiddenToken(shellAccountingPageText, token)) {
            std::cerr << "forbidden runtime token found after presenter exposure: `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
