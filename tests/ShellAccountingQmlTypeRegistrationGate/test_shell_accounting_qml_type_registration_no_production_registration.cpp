#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <iostream>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const std::vector<std::string> qmlTokens{
        "ShellAccountingPresenter",
        "accountingPresenter",
        "ETFDecisionTerminal.ShellAccounting",
    };
    if (containsAnyToken(productionQmlFiles(root), qmlTokens)) {
        return 1;
    }

    const std::vector<std::string> registrationTokens{
        "qmlRegisterType<ShellAccountingPresenter>",
        "qmlRegisterType<ShellAccountingStatusObject>",
        "qmlRegisterType<ShellAccountingIssueListModel>",
        "qmlRegisterType<ShellPositionListModel>",
        "qmlRegisterSingletonType<ShellAccountingPresenter>",
        "qmlRegisterSingletonType<ShellAccountingStatusObject>",
        "qmlRegisterSingletonType<ShellAccountingIssueListModel>",
        "qmlRegisterSingletonType<ShellPositionListModel>",
        "ETFDecisionTerminal.ShellAccounting",
    };
    if (containsAnyToken(productionRegistrationFiles(root), registrationTokens)) {
        return 1;
    }
    return 0;
}
