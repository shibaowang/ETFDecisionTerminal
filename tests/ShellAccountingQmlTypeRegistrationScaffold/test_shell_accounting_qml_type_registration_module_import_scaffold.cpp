#include "ShellAccountingQmlTypeRegistrationScenario.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration;

int main()
{
    const auto module = moduleBoundaryScenario();
    if (module.moduleName != "ETFDecisionTerminal.ShellAccounting") {
        std::cerr << "Unexpected ShellAccounting QML module name: " << module.moduleName << '\n';
        return 1;
    }
    if (!module.allowedInFirstStage || !module.readOnly) {
        std::cerr << "Module boundary must be read-only.\n";
        return 1;
    }
    if (module.exposesServiceDependency || module.exposesEngineDependency ||
        module.exposesTradeMethod || module.exposesWriteMethod) {
        std::cerr << "Module boundary exposes forbidden dependency or action.\n";
        return 1;
    }
    if (module.notes.find("does not imply production QML binding") == std::string::npos) {
        std::cerr << "Module boundary must state that import does not imply production QML binding.\n";
        return 1;
    }
    return 0;
}
