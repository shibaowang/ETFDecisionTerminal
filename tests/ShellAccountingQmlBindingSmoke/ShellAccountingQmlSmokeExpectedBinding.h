#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_binding_smoke {

struct ExpectedBindingObject final {
    std::string name;
    bool readOnly = true;
    bool futureOnly = false;
};

[[nodiscard]] std::vector<ExpectedBindingObject> allowedBindingObjects();
[[nodiscard]] std::vector<std::string> forbiddenBindingTargets();
[[nodiscard]] std::vector<std::string> forbiddenTradeUiTokens();
[[nodiscard]] std::vector<std::string> requiredStaticGateTests();

}  // namespace etfdt::tests::shell_accounting_qml_binding_smoke
