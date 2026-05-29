#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(root / "docs" / "74_shell_accounting_production_qml_binding_implementation.md");
    for (const auto& token : {
             "rollback",
             "remove ShellAccountingReadOnlyPage.qml",
             "remove the ContentHost mapping",
             "remove the shell_accounting registry entry",
             "must not call DataServiceClient",
             "must not fake data",
             "must not write",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "docs/74 rollback section missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
