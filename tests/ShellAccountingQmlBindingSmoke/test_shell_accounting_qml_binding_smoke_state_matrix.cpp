#include "ShellAccountingQmlSmokeScenario.h"

#include <algorithm>
#include <iostream>
#include <string>

using namespace etfdt::tests::shell_accounting_qml_binding_smoke;

namespace {

bool hasState(const std::string& state)
{
    const auto scenarios = stateMatrixScenarios();
    return std::any_of(scenarios.begin(), scenarios.end(), [&](const auto& scenario) {
        return scenario.state == state && scenario.expectedVisibleState == state;
    });
}

}  // namespace

int main()
{
    const auto scenarios = stateMatrixScenarios();
    for (const auto& required : {"Idle", "Loading", "Loaded", "Empty", "Unavailable", "Warning", "Error", "Stale"}) {
        if (!hasState(required)) {
            std::cerr << "Missing visible state scenario: " << required << '\n';
            return 1;
        }
    }

    const auto empty = std::find_if(scenarios.begin(), scenarios.end(), [](const auto& scenario) {
        return scenario.state == "Empty";
    });
    const auto unavailable = std::find_if(scenarios.begin(), scenarios.end(), [](const auto& scenario) {
        return scenario.state == "Unavailable";
    });
    if (empty == scenarios.end() || unavailable == scenarios.end()) {
        return 1;
    }
    if (empty->expectedVisibleState == unavailable->expectedVisibleState) {
        std::cerr << "Empty and Unavailable must stay distinct.\n";
        return 1;
    }
    if (unavailable->implemented || unavailable->expectedVisibleState != "Unavailable") {
        std::cerr << "implemented=false must map to Unavailable.\n";
        return 1;
    }

    for (const auto& scenario : scenarios) {
        if (!scenario.readOnly || scenario.writeEnabled) {
            std::cerr << "State scenario violates read-only boundary: " << scenario.scenarioName << '\n';
            return 1;
        }
        if (scenario.state == "Warning" && scenario.issues.empty()) {
            std::cerr << "Warning state must keep issue visible.\n";
            return 1;
        }
        if (scenario.state == "Error" && scenario.expectedVisibleState != "Error") {
            std::cerr << "Error state must stay visible.\n";
            return 1;
        }
        if (scenario.state == "Stale" && scenario.expectedVisibleState == "Loaded") {
            std::cerr << "Stale must not be treated as OK / Loaded.\n";
            return 1;
        }
    }
    return 0;
}
