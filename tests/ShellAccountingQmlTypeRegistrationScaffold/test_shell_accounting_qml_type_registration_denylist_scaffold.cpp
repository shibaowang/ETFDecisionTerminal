#include "ShellAccountingQmlTypeRegistrationExpectedType.h"
#include "ShellAccountingQmlTypeRegistrationScenario.h"

#include <algorithm>
#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_type_registration;

int main()
{
    const auto scenarios = forbiddenTypeScenarios();
    const auto forbiddenNames = forbiddenTypeNames();
    if (scenarios.size() != forbiddenNames.size()) {
        std::cerr << "Forbidden type scenario count does not match forbidden type names.\n";
        return 1;
    }

    for (const auto& required : forbiddenNames) {
        const auto found = std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
            return scenario.typeName == required && !scenario.allowedInFirstStage;
        });
        if (!found) {
            std::cerr << "Missing forbidden type descriptor: " << required << '\n';
            return 1;
        }
    }

    for (const auto& scenario : scenarios) {
        if (scenario.allowedInFirstStage || scenario.readOnly) {
            std::cerr << "Forbidden type is accidentally allowed or marked read-only: "
                      << scenario.typeName << '\n';
            return 1;
        }
        if (scenario.notes.empty()) {
            std::cerr << "Forbidden type requires a reason: " << scenario.typeName << '\n';
            return 1;
        }
    }
    return 0;
}
