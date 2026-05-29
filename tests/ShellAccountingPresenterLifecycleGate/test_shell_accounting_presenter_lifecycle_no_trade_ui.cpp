#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto pageText = readTextFile(shellAccountingReadOnlyPage(root));
    for (const auto& token : forbiddenTradeUiTokens()) {
        if (pageText.find(token) != std::string::npos) {
            std::cerr << "ShellAccounting QML page exposes trading token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
