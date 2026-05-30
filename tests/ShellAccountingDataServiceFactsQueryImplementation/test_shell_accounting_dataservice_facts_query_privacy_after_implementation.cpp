#include "ShellAccountingDataServiceFactsQueryImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto dataServiceSource = readTextFile(dataServiceActionsPath(root));
    for (const auto& token : {
             "rawSqlExposed\\\":false",
             "rawTradeLogPayloadExposed\\\":false",
             "without exposing raw SQL or payload",
         }) {
        if (dataServiceSource.find(token) == std::string::npos) {
            std::cerr << "DataService payload privacy token missing `" << token << "`\n";
            return 1;
        }
    }
    if (containsTokenInFiles(shellAccountingProductionBoundaryFiles(root), "rawSqlExposed\\\":true")
        || containsTokenInFiles(shellAccountingProductionBoundaryFiles(root), "rawTradeLogPayloadExposed\\\":true")) {
        return 1;
    }
    return 0;
}
