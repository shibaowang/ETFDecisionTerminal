#include "ShellAccountingQmlStartupRegistrationWiringGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto helper = shellAccountingQmlRegistrationHelperText(root);

    for (const auto& token : {
             "qmlRegisterUncreatableType<T>",
             "ShellAccountingPresenter",
             "ShellAccountingStatusObject",
             "ShellAccountingIssueListModel",
             "ShellPositionListModel",
             "ETFDecisionTerminal.ShellAccounting",
         }) {
        if (helper.find(token) == std::string::npos) {
            std::cerr << "registration helper missing readonly allowlist token `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "qmlRegisterType<ShellAccounting",
             "qmlRegisterSingletonType<ShellAccounting",
             "ShellCashSummaryObject",
             "ShellPortfolioPnlObject",
             "ShellBasePositionObject",
             "ShellSniperPoolObject",
             "ShellSniperTierListModel",
         }) {
        if (helper.find(token) != std::string::npos) {
            std::cerr << "registration helper must preserve first-stage uncreatable allowlist; found `"
                      << token << "`\n";
            return 1;
        }
    }
    return 0;
}
