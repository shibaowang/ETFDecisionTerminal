#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding {

struct ShellAccountingDataServiceAdapterExpectedCall final {
    std::string fetchMethod;
    std::string wrapperMethod;
    std::string actionName;
    bool readOnly = true;
};

[[nodiscard]] std::vector<ShellAccountingDataServiceAdapterExpectedCall>
expectedReadOnlyAdapterCalls();

[[nodiscard]] std::vector<std::string> readOnlyActionAllowlist();
[[nodiscard]] std::vector<std::string> forbiddenActionDenylist();
[[nodiscard]] bool isReadOnlyAccountingActionAllowed(const std::string& actionName);

}  // namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding
