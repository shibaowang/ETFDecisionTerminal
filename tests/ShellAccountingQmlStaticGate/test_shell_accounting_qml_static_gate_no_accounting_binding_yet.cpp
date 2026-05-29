#include "QmlStaticGateScanner.h"

#include <algorithm>
#include <iostream>
#include <iterator>
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
    std::vector<QmlHit> disallowedHits;
    std::copy_if(hits.begin(), hits.end(), std::back_inserter(disallowedHits), [](const auto& hit) {
        const auto filename = hit.path.filename().string();
        const auto authorizedShellAccountingPage = filename == "ShellAccountingReadOnlyPage.qml";
        const auto authorizedPresenterPlumbing =
            (filename == "Main.qml" || filename == "AppShell.qml" || filename == "ContentHost.qml") &&
            hit.token == "accountingPresenter";
        const auto authorizedPresenterProperty =
            hit.token == "ShellAccountingPresenter" || hit.token == "accountingPresenter";
        return !((authorizedShellAccountingPage && authorizedPresenterProperty) || authorizedPresenterPlumbing);
    });

    if (!disallowedHits.empty()) {
        std::cerr << "ShellAccounting QML binding is only authorized for the TASK-131 read-only shell.\n";
        printHits(disallowedHits);
        return 1;
    }
    return 0;
}
