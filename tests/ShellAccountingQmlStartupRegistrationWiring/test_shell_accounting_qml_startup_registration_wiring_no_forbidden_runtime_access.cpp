#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (containsForbiddenExposure(productionAppFiles(root), forbiddenRuntimeAccessTokens())) {
        std::cerr << "startup wiring and production QML must not expose forbidden runtime access\n";
        return 1;
    }
    return 0;
}
