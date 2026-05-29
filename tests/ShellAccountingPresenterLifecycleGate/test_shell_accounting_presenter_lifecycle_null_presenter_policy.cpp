#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs75 = readTextFile(root / "docs" / "75_shell_accounting_presenter_lifecycle_gate.md");
    const auto docs76 = readTextFile(root / "docs" / "76_shell_accounting_presenter_lifecycle_test_plan.md");
    const auto docs74 = readTextFile(root / "docs" / "74_shell_accounting_production_qml_binding_implementation.md");
    const auto combined = docs75 + "\n" + docs76 + "\n" + docs74;
    for (const auto& token : {
             "accountingPresenter",
             "null",
             "unavailable",
             "must not fake data",
             "must not call DataServiceClient",
             "must not write",
             "must not generate TradeDraft",
             "must not show trading UI",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "null presenter policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
