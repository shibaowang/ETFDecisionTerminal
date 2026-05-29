#include "ShellAccountingProductionQmlBindingGate.h"

#include <algorithm>
#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (containsForbiddenExposure(productionStartupFiles(root), contextExposureTokens())) {
        std::cerr << "production startup must not expose accounting presenter or direct services\n";
        return 1;
    }
    const auto authorizedText = readTextFile(authorizedShellAccountingQmlBindingFile(root));
    if (authorizedText.find("property ShellAccountingPresenter accountingPresenter: null") == std::string::npos) {
        std::cerr << "authorized QML shell must keep nullable presenter property only\n";
        return 1;
    }
    auto qmlFiles = productionQmlFiles(root);
    const auto authorizedFile = authorizedShellAccountingQmlBindingFile(root);
    qmlFiles.erase(
        std::remove(qmlFiles.begin(), qmlFiles.end(), authorizedFile),
        qmlFiles.end());
    if (containsForbiddenExposure(qmlFiles, contextExposureTokens())) {
        std::cerr << "production QML must not expose direct accounting dependencies\n";
        return 1;
    }
    for (const auto& token : {
             "ShellAccountingReadOnlyController",
             "ShellAccountingDataServiceAdapter",
             "ShellAccountingDataServiceClientPortAdapter",
             "DataServiceClient",
             "AccountingEngine",
             "SQLite",
             "DataAccess",
         }) {
        if (authorizedText.find(token) != std::string::npos) {
            std::cerr << "authorized QML shell must not expose direct accounting dependency `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
