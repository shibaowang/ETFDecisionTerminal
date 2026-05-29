#include "ShellAccountingQmlTypeRegistrationExpectedType.h"
#include "ShellAccountingQmlTypeRegistrationScenario.h"

#include <algorithm>
#include <iostream>
#include <string>

using namespace etfdt::tests::shell_accounting_qml_type_registration;

namespace {

bool contains(const std::vector<std::string>& values, const std::string& value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

}  // namespace

int main()
{
    const auto scenarios = firstStageAllowedTypes();
    const auto expectedTypes = firstStageExpectedTypes();
    if (scenarios.size() != 4 || expectedTypes.size() != 4) {
        std::cerr << "Expected exactly four first-stage ShellAccounting QML registration candidates.\n";
        return 1;
    }

    for (const auto& scenario : scenarios) {
        if (!scenario.allowedInFirstStage || !scenario.readOnly) {
            std::cerr << "Allowed type is not read-only: " << scenario.typeName << '\n';
            return 1;
        }
        if (scenario.exposesWriteMethod || scenario.exposesTradeMethod ||
            scenario.exposesServiceDependency || scenario.exposesEngineDependency) {
            std::cerr << "Allowed type exposes a forbidden capability: " << scenario.typeName << '\n';
            return 1;
        }
        for (const auto& gate : {"TASK-119", "TASK-121", "TASK-122"}) {
            if (!contains(scenario.requiredGates, gate)) {
                std::cerr << "Allowed type is missing required gate " << gate << ": "
                          << scenario.typeName << '\n';
                return 1;
            }
        }
    }

    for (const auto& required : {
             "ShellAccountingPresenter",
             "ShellAccountingStatusObject",
             "ShellAccountingIssueListModel",
             "ShellPositionListModel",
         }) {
        const auto found = std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
            return scenario.typeName == required;
        });
        if (!found) {
            std::cerr << "Missing first-stage allowed type: " << required << '\n';
            return 1;
        }
    }
    return 0;
}
