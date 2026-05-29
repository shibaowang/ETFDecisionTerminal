#include "ShellAccountingPresenterLifecycleImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_presenter_lifecycle_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto pageText = readTextFile(shellAccountingReadOnlyPage(root));
    for (const auto& token : {
             "property ShellAccountingPresenter accountingPresenter: null",
             "presenterAvailable",
             "UNAVAILABLE",
             "read-only",
             "real accounting data is not connected",
         }) {
        if (pageText.find(token) == std::string::npos) {
            std::cerr << "ShellAccounting page missing unavailable-safe token `" << token << "`\n";
            return 1;
        }
    }
    const auto docs77 = readTextFile(root / "docs" / "77_shell_accounting_presenter_lifecycle_implementation.md");
    for (const auto& token : {
             "must not fake data",
             "must not call DataServiceClient",
             "must not access SQLite",
             "must not generate TradeDraft",
             "must not show trading UI",
         }) {
        if (docs77.find(token) == std::string::npos) {
            std::cerr << "docs/77 missing unavailable fallback token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
