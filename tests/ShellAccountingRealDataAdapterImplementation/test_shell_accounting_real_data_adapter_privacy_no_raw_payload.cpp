#include "ShellAccountingRealDataAdapterImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlFiles = productionQmlFiles(root);
    const auto docs80 = readTextFile(docs80Path(root));

    if (containsTokenInFiles(qmlFiles, "rawPayload") ||
        containsTokenInFiles(qmlFiles, "trade_log") ||
        containsTokenInFiles(qmlFiles, "SELECT ")) {
        std::cerr << "production QML must not expose raw payload, trade_log, or SQL\n";
        return 1;
    }
    for (const auto& phrase : {
             "no raw SQL in logs",
             "no full trade_log payload in QML",
             "no sensitive raw payload",
             "no raw transport payload",
         }) {
        if (docs80.find(phrase) == std::string::npos) {
            std::cerr << "docs/80 missing privacy phrase `" << phrase << "`\n";
            return 1;
        }
    }
    return 0;
}
