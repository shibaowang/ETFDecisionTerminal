#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto actionRegion = dataServiceAccountingActionRegion(root);
    if (actionRegion.empty()) {
        std::cerr << "could not locate DataService ShellAccounting action region\n";
        return 1;
    }

    for (const auto& action : authorizedReadOnlyActions()) {
        if (actionRegion.find(action) == std::string::npos) {
            std::cerr << "missing authorized action `" << action << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "\"implemented\":false",
             "\"dataSourceAccessed\":false",
             "\"sqliteAccessed\":false",
             "\"accountingEngineCalled\":false",
             "NO_SQLITE_FACTS_QUERY",
             "NO_ACCOUNTING_ENGINE_CALL",
         }) {
        if (actionRegion.find(token) == std::string::npos) {
            std::cerr << "current guard action region missing no-query token `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "SELECT ",
             "sqlite3_",
             "AccountingTradeFactReader",
             "AccountingReplayEngine",
             "replayFromFacts",
         }) {
        if (actionRegion.find(token) != std::string::npos) {
            std::cerr << "current guard action region contains future query/replay token `" << token << "`\n";
            return 1;
        }
    }

    const auto boundaryFiles = shellAccountingProductionBoundaryFiles(root);
    for (const auto& token : {
             "SQLiteConnection",
             "AccountingTradeFactReader",
             "AccountingReplayEngine",
             "replayFromFacts",
         }) {
        if (containsTokenInFiles(boundaryFiles, token)) {
            std::cerr << "ShellAccounting production path contains direct facts query token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
