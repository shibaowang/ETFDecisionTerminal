#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs78Path(root)) + "\n" + readTextFile(docs79Path(root));
    for (const auto& token : {
             "no raw SQL in logs",
             "no full trade_log payload in QML",
             "no sensitive raw payload in issue text",
             "privacy mode remains respected",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "privacy policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
