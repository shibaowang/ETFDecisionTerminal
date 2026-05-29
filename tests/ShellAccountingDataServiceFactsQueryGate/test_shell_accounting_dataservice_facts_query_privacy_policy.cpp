#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs81Path(root)) + "\n" + readTextFile(docs82Path(root));
    for (const auto& token : {
             "no raw SQL in QML",
             "no full trade_log payload in QML",
             "no sensitive raw payload in issue text",
             "no raw facts dump in default logs",
             "no full account cash history in QML error",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "facts query privacy policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
