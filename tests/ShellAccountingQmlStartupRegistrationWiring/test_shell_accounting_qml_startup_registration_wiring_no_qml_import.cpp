#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlFiles = productionQmlFiles(root);

    for (const auto& token : {
             "import ETFDecisionTerminal.ShellAccounting",
             "accountingPresenter",
             "ShellAccountingPresenter",
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
            std::cerr << "production QML must remain unbound after startup registration wiring\n";
            return 1;
        }
    }
    return 0;
}
