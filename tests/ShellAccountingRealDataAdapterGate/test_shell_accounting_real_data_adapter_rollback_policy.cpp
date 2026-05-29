#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs78Path(root)) + "\n" + readTextFile(docs79Path(root));
    for (const auto& token : {
             "rollback",
             "keep presenter unavailable",
             "must not fake data",
             "must not write DB",
             "must not generate TradeDraft",
             "must not bypass DataService with SQLite direct reads",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "rollback policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
