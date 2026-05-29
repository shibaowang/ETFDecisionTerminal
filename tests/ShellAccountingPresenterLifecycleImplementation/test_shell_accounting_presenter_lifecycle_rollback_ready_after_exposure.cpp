#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs75 = readTextFile(root / "docs" / "75_shell_accounting_presenter_lifecycle_gate.md");
    const auto docs76 = readTextFile(root / "docs" / "76_shell_accounting_presenter_lifecycle_test_plan.md");
    const auto docs77 = readTextFile(root / "docs" / "77_shell_accounting_presenter_lifecycle_implementation.md");
    const auto combined = docs75 + docs76 + docs77;
    for (const auto& token : {
             "rollback",
             "disable context exposure",
             "must not call DataServiceClient",
             "must not fake data",
             "must not write",
             "must not generate TradeDraft",
             "presenter construction",
             "context property",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "rollback documentation missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
