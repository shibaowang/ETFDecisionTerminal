#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    const auto page = readTextFile(authorizedPagePath(sourceRoot(argc, argv)));
    for (const auto& token : {
             "accountingPresenter: null",
             "UNAVAILABLE",
             "\xE7\xA6\x81\xE7\x94\xA8",
             "unavailable-safe",
             "not displayed",
         }) {
        if (page.find(token) == std::string::npos) {
            std::cerr << "authorized page missing unavailable state token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
