#include "QmlStaticGateScanner.h"

#include <iostream>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_static_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto files = loadQmlFiles(root);
    const std::vector<std::string> tokens{
        "data.audit.append",
        "writeEnabled: true",
        "writeEnabled=true",
        "INSERT INTO",
        "UPDATE",
        "DELETE FROM",
        "CREATE TABLE",
        "DROP TABLE",
        "ALTER TABLE",
        "cash_snapshot",
        "position_snapshot",
        "portfolio_summary",
        "trade_log",
        "trade_execution_group",
    };

    const auto hits = scanTokens(files, tokens);
    if (!hits.empty()) {
        std::cerr << "QML must not contain database write or write-enabled action tokens.\n";
        printHits(hits);
        return 1;
    }
    return 0;
}
