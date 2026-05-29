#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto mainText = readTextFile(root / "apps" / "ETFDecisionShell" / "src" / "main.cpp");
    const auto presenterOffset = mainText.find("ShellAccountingPresenter shellAccountingPresenter");
    const auto engineOffset = mainText.find("QQmlApplicationEngine engine");
    if (presenterOffset == std::string::npos || engineOffset == std::string::npos || presenterOffset > engineOffset) {
        std::cerr << "presenter must be stack-owned before QQmlApplicationEngine so engine is destroyed first\n";
        return 1;
    }
    if (mainText.find("new ShellAccountingPresenter") != std::string::npos) {
        std::cerr << "presenter must not use anonymous raw new\n";
        return 1;
    }
    const auto docs77 = readTextFile(root / "docs" / "77_shell_accounting_presenter_lifecycle_implementation.md");
    for (const auto& token : {
             "stack-owned",
             "QQmlApplicationEngine is created after the presenter",
             "engine is destroyed before the presenter",
         }) {
        if (docs77.find(token) == std::string::npos) {
            std::cerr << "docs/77 missing ownership token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
