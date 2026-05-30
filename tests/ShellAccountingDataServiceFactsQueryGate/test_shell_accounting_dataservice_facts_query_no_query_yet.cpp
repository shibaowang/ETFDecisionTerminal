#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto dataServiceSource = readTextFile(dataServiceActionsPath(root));
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
             "ShellAccountingReadOnlyFactsQuery",
             "shellAccountingPayloadPrefix",
             "shellAccountingEmptyPayload",
             "shellAccountingQueryErrorPayload",
         }) {
        if (actionRegion.find(token) == std::string::npos) {
            std::cerr << "authorized read-only query region missing token `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "\\\"implemented\\\":true",
             "\\\"dataSourceAccessed\\\":true",
             "\\\"sqliteAccessed\\\":true",
             "\\\"accountingEngineCalled\\\":",
             "\\\"snapshotRebuilt\\\":false",
             "\\\"readOnly\\\":true",
             "\\\"writeEnabled\\\":false",
         }) {
        if (dataServiceSource.find(token) == std::string::npos) {
            std::cerr << "DataService action source missing read-only query payload token `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "sqlite3_",
             "replayFromFacts",
         }) {
        if (actionRegion.find(token) != std::string::npos) {
            std::cerr << "authorized read-only query region contains forbidden token `" << token << "`\n";
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
