#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs81Path(root)) + "\n" + readTextFile(docs82Path(root));
    for (const auto& token : {
             "INSERT",
             "UPDATE",
             "DELETE",
             "REPLACE",
             "DROP",
             "ALTER",
             "CREATE TABLE",
             "transaction write",
             "write repository",
             "audit append",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "no-write SQL policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
