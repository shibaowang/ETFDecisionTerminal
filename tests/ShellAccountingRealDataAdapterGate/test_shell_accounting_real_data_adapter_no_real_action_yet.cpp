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

    for (const auto& token : {
             "ShellAccountingDataServiceAdapter",
             "ShellAccountingDataServiceClientPortAdapter",
             "DataServiceClient",
             "setServiceAdapter(",
         }) {
        if (containsTokenInFiles(startupFiles, token)) {
            std::cerr << "production startup has real adapter wiring token `" << token << "`\n";
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
