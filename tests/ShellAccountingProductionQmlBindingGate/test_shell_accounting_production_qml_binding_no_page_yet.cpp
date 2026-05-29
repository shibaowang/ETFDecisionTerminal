#include "ShellAccountingProductionQmlBindingGate.h"

#include <filesystem>
#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlFiles = productionQmlFiles(root);
    const auto authorizedFile = authorizedShellAccountingQmlBindingFile(root);
    if (!std::filesystem::exists(authorizedFile)) {
        std::cerr << "authorized ShellAccounting read-only page is missing\n";
        return 1;
    }
    const auto pageText = readTextFile(authorizedFile);
    if (pageText.find("objectName: \"shellAccountingReadOnlyPage\"") == std::string::npos ||
        pageText.find("UNAVAILABLE") == std::string::npos ||
        pageText.find("read-only") == std::string::npos) {
        std::cerr << "authorized ShellAccounting page must stay read-only and unavailable-safe\n";
        return 1;
    }
    for (const auto& token : {
             "ShellAccountingDashboard",
             "ShellAccountingPanel",
             "AccountingDashboard",
             "buyButton",
             "sellButton",
             "createTradeDraft",
             "brokerOrder",
             "strategyExecute",
         }) {
        if (containsToken(qmlFiles, token)) {
            std::cerr << "production QML must not add unapproved ShellAccounting page/component/runtime controls\n";
            return 1;
        }
    }
    return 0;
}
