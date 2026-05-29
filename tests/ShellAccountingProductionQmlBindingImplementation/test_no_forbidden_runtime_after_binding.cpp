#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    if (containsForbiddenToken(productionAppFiles(sourceRoot(argc, argv)), forbiddenRuntimeTokens())) {
        std::cerr << "production app must not expose forbidden accounting runtime access after binding shell\n";
        return 1;
    }
    return 0;
}
