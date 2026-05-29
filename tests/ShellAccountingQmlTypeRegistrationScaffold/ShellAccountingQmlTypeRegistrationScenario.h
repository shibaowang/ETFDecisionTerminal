#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_type_registration {

struct ShellAccountingQmlTypeRegistrationScenario final {
    std::string typeName;
    std::string moduleName;
    std::string phase;
    bool allowedInFirstStage = false;
    bool readOnly = true;
    bool exposesWriteMethod = false;
    bool exposesTradeMethod = false;
    bool exposesServiceDependency = false;
    bool exposesEngineDependency = false;
    std::vector<std::string> requiredGates;
    std::string notes;
};

[[nodiscard]] std::vector<ShellAccountingQmlTypeRegistrationScenario> firstStageAllowedTypes();
[[nodiscard]] std::vector<ShellAccountingQmlTypeRegistrationScenario> forbiddenTypeScenarios();
[[nodiscard]] ShellAccountingQmlTypeRegistrationScenario moduleBoundaryScenario();
[[nodiscard]] std::vector<std::string> requiredRegistrationGates();
[[nodiscard]] std::string registrationModuleName();

}  // namespace etfdt::tests::shell_accounting_qml_type_registration
