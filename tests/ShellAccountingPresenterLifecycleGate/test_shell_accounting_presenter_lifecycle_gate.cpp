#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs75 = readTextFile(root / "docs" / "75_shell_accounting_presenter_lifecycle_gate.md");
    const auto docs76 = readTextFile(root / "docs" / "76_shell_accounting_presenter_lifecycle_test_plan.md");
    for (const auto& token : preGateTokens()) {
        if (docs75.find(token) == std::string::npos || docs76.find(token) == std::string::npos) {
            std::cerr << "presenter lifecycle docs missing pre-gate `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "user authorization",
             "Presenter lifecycle is not real accounting action",
             "Context exposure is not DataService / SQLite / AccountingEngine integration",
         }) {
        if (docs75.find(token) == std::string::npos) {
            std::cerr << "docs/75 missing gate token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
