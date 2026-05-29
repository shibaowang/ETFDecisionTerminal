#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    if (containsForbiddenToken(productionQmlFiles(sourceRoot(argc, argv)), tradeUiTokens())) {
        std::cerr << "production QML binding shell must not expose trading UI controls\n";
        return 1;
    }
    return 0;
}
