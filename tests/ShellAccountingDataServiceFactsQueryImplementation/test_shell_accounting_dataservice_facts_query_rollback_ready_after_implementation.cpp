#include "ShellAccountingDataServiceFactsQueryImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs81Path(root)) + "\n"
        + readTextFile(docs82Path(root)) + "\n"
        + readTextFile(docs83Path(root));
    for (const auto& token : {
             "rollback",
             "disable facts query",
             "keep presenter unavailable",
             "must not fake data",
             "must not write DB",
             "must not generate TradeDraft",
             "must not call AccountingEngine replay",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "rollback docs missing `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
