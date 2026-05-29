#include "ShellAccountingQmlSmokeExpectedBinding.h"

#include <algorithm>
#include <iostream>
#include <string>

using namespace etfdt::tests::shell_accounting_qml_binding_smoke;

namespace {

bool hasBinding(const std::string& name)
{
    const auto objects = allowedBindingObjects();
    return std::any_of(objects.begin(), objects.end(), [&](const auto& object) {
        return object.name == name && object.readOnly;
    });
}

}  // namespace

int main()
{
    const auto objects = allowedBindingObjects();
    if (objects.size() != 9) {
        std::cerr << "Expected nine allowed ShellAccounting binding object descriptors.\n";
        return 1;
    }

    for (const auto& object : objects) {
        if (!object.readOnly) {
            std::cerr << "Binding object is not read-only: " << object.name << '\n';
            return 1;
        }
    }

    for (const auto& required : {
             "ShellAccountingPresenter",
             "ShellAccountingStatusObject",
             "ShellAccountingIssueListModel",
             "ShellPositionListModel",
             "ShellCashSummaryObject",
             "ShellPortfolioPnlObject",
             "ShellBasePositionObject",
             "ShellSniperPoolObject",
             "ShellSniperTierListModel",
         }) {
        if (!hasBinding(required)) {
            std::cerr << "Missing allowed read-only binding object: " << required << '\n';
            return 1;
        }
    }

    for (const auto& forbidden : forbiddenBindingTargets()) {
        if (forbidden.empty()) {
            std::cerr << "Forbidden binding target descriptor must not be empty.\n";
            return 1;
        }
    }
    return 0;
}
