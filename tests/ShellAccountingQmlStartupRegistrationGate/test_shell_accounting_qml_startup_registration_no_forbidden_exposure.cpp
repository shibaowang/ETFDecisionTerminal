#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (containsForbiddenExposure(productionAppFiles(root), forbiddenExposureTokens())) {
        std::cerr << "production startup/QML must not expose forbidden accounting dependencies or actions\n";
        return 1;
    }
    return 0;
}
