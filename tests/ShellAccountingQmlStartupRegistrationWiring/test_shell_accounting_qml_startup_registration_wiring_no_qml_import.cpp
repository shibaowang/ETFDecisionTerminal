#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlFiles = productionQmlFiles(root);

    for (const auto& token : {
             "ShellAccountingStatusObject",
             "ShellAccountingIssueListModel",
             "ShellPositionListModel",
             "buyButton",
             "sellButton",
             "createTradeDraft",
             "brokerOrder",
             "strategyExecute",
        }) {
        if (containsToken(qmlFiles, token)) {
            std::cerr << "production QML startup wiring boundary must not expose extra ShellAccounting bindings\n";
            return 1;
        }
    }
    if (countTokenInFiles(qmlFiles, "import ETFDecisionTerminal.ShellAccounting") != 1 ||
        countTokenInFiles(qmlFiles, "property ShellAccountingPresenter accountingPresenter: null") != 1) {
        std::cerr << "TASK-131 permits exactly one read-only ShellAccounting import and nullable property\n";
        return 1;
    }
    return 0;
}
