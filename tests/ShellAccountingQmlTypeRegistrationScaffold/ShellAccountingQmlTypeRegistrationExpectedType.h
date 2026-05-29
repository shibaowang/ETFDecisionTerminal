#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_type_registration {

struct ShellAccountingQmlTypeRegistrationExpectedType final {
    std::string typeName;
    bool allowed = false;
    std::string reason;
    std::vector<std::string> forbiddenMethods;
    std::vector<std::string> forbiddenProperties;
    std::vector<std::string> requiredGates;
};

[[nodiscard]] std::vector<ShellAccountingQmlTypeRegistrationExpectedType> firstStageExpectedTypes();
[[nodiscard]] std::vector<std::string> forbiddenTypeNames();
[[nodiscard]] std::vector<std::string> forbiddenMethodTokens();
[[nodiscard]] std::vector<std::string> forbiddenPropertyTokens();

}  // namespace etfdt::tests::shell_accounting_qml_type_registration
