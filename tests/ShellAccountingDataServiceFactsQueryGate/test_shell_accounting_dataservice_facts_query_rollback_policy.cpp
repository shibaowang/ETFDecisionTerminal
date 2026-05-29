#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs81Path(root)) + "\n" + readTextFile(docs82Path(root));
    for (const auto& token : {
             "rollback",
             "keep presenter unavailable",
             "Fallback must not fake data",
             "Fallback must not write DB",
             "Fallback must not generate TradeDraft",
             "Fallback must not bypass DataService with shell-side SQLite reads",
             "Fallback must not call AccountingEngine replay",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "facts query rollback policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
