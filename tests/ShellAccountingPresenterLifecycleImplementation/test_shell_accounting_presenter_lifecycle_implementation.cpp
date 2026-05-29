#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs77Path = root / "docs" / "77_shell_accounting_presenter_lifecycle_implementation.md";
    if (!std::filesystem::exists(docs77Path)) {
        std::cerr << "docs/77 missing\n";
        return 1;
    }
    const auto docs77 = readTextFile(docs77Path);
    for (const auto& token : {
             "TASK-133",
             "presenter lifecycle is not real accounting action",
             "context exposure is not DataService / SQLite / AccountingEngine integration",
             "accountingPresenter",
             "rollback",
         }) {
        if (docs77.find(token) == std::string::npos) {
            std::cerr << "docs/77 missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
