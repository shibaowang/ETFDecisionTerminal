#include "QmlStaticGateScanner.h"

#include <iostream>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_static_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto files = loadQmlFiles(root);
    const std::vector<std::string> tokens{
        "DataServiceClient",
        "DataServiceApi",
        "AccountingEngine",
        "DataAccess",
        "SQLite",
        "SQLiteConnection",
        "AccountingReplayEngine",
        "AccountingTradeFactReader",
        "ShellAccountingDataServiceAdapter",
        "ShellAccountingDataServiceClientPortAdapter",
    };

    const auto hits = scanTokens(files, tokens);
    if (!hits.empty()) {
        std::cerr << "QML must not directly reference service, database, or accounting engine dependencies.\n";
        printHits(hits);
        return 1;
    }
    return 0;
}
