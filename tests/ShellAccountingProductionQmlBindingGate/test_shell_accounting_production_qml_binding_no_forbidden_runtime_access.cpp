#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (containsForbiddenExposure(productionAppFiles(root), forbiddenRuntimeAccessTokens())) {
        std::cerr << "production QML/startup must not expose write, trade, or draft runtime access\n";
        return 1;
    }
    return 0;
}
