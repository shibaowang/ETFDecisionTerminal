#include "QmlStaticGateScanner.h"

#include <iostream>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_static_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto files = loadQmlFiles(root);
    const std::vector<std::string> tokens{
        "ShellAccountingPresenter",
        "accountingPresenter",
        "ShellAccountingStatusObject",
        "ShellAccountingIssueListModel",
        "ShellPositionListModel",
        "ShellCashSummaryObject",
        "ShellPortfolioPnlObject",
        "ShellBasePositionObject",
        "ShellSniperPoolObject",
        "ShellSniperTierListModel",
    };

    const auto hits = scanTokens(files, tokens);
    if (!hits.empty()) {
        std::cerr << "ShellAccounting QML binding is not authorized in TASK-119.\n";
        printHits(hits);
        return 1;
    }
    return 0;
}
