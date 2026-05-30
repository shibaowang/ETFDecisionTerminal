#include "ShellAccountingDataServiceFactsQueryImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs83 = readTextFile(docs83Path(root));
    if (docs83.find("TASK-137") == std::string::npos
        || docs83.find("read-only actions") == std::string::npos
        || docs83.find("no write SQL") == std::string::npos
        || docs83.find("no AccountingEngine replay") == std::string::npos) {
        std::cerr << "docs/83 does not document TASK-137 read-only facts query boundaries\n";
        return 1;
    }

    const auto source = readTextFile(dataServiceActionsPath(root));
    for (const auto& action : authorizedActions()) {
        if (source.find(action) == std::string::npos) {
            std::cerr << "DataService action source missing authorized action " << action << '\n';
            return 1;
        }
    }
    if (source.find("ShellAccountingReadOnlyFactsQuery") == std::string::npos) {
        std::cerr << "DataService action source does not call read-only facts query boundary\n";
        return 1;
    }
    return 0;
}
