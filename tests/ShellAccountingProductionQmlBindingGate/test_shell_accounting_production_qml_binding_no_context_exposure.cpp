#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (containsForbiddenExposure(productionAppFiles(root), contextExposureTokens())) {
        std::cerr << "production startup/QML must not expose accounting presenter or direct services yet\n";
        return 1;
    }
    return 0;
}
