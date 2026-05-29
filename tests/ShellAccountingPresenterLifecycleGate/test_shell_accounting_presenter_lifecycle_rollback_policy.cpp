#include "ShellAccountingPresenterLifecycleGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs75 = readTextFile(root / "docs" / "75_shell_accounting_presenter_lifecycle_gate.md");
    const auto docs76 = readTextFile(root / "docs" / "76_shell_accounting_presenter_lifecycle_test_plan.md");
    const auto docs74 = readTextFile(root / "docs" / "74_shell_accounting_production_qml_binding_implementation.md");
    for (const auto& token : {
             "rollback",
             "Fallback must keep `accountingPresenter` null",
             "Fallback must not call DataServiceClient",
             "Fallback must not fake data",
             "Fallback must not write",
             "Fallback must not generate TradeDraft",
             "future presenter construction, context property",
         }) {
        if (docs75.find(token) == std::string::npos) {
            std::cerr << "docs/75 rollback policy missing token `" << token << "`\n";
            return 1;
        }
    }
    if (docs76.find("Rollback strategy documented") == std::string::npos ||
        docs74.find("rollback") == std::string::npos) {
        std::cerr << "presenter lifecycle rollback references are incomplete\n";
        return 1;
    }
    return 0;
}
