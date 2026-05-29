#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    const std::vector<std::filesystem::path> qmlFiles = {
        root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml",
    };
    const auto presenterFiles = presenterCoreFiles(root);
    const auto startupText = readTextFile(root / "apps" / "ETFDecisionShell" / "src" / "main.cpp");

    if (countTokenInFiles(startupFiles, "ShellAccountingDataServiceAdapter") == 0 ||
        countTokenInFiles(startupFiles, "ShellAccountingDataServiceClientPortAdapter") == 0 ||
        countTokenInFiles(startupFiles, "DataServiceClient") == 0 ||
        countTokenInFiles(startupFiles, "setServiceAdapter(") != 1) {
        std::cerr << "production startup must contain exactly one authorized read-only adapter wiring path\n";
        return 1;
    }
    for (const auto& token : {
             "createTradeDraft",
             "brokerOrder(",
             "strategyExecute",
             "data.audit.append",
             "writeEnabled: true",
         }) {
        if (startupText.find(token) != std::string::npos) {
            std::cerr << "production startup must not contain write/trade token `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "DataServiceClient",
             "serviceAction",
             "socket",
             "fetch(",
         }) {
        if (containsTokenInFiles(qmlFiles, token)) {
            std::cerr << "production QML has direct service token `" << token << "`\n";
            return 1;
        }
    }
    if (containsTokenInFiles(presenterFiles, "DataServiceClient")) {
        std::cerr << "presenter/controller core must not call DataServiceClient directly\n";
        return 1;
    }
    return 0;
}
