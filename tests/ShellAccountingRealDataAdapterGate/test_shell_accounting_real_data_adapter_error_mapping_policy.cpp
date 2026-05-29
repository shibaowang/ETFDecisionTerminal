#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(docs78Path(root)) + "\n" + readTextFile(docs79Path(root));
    for (const auto& token : {
             "timeout mapping",
             "transport error mapping",
             "protocol error mapping",
             "unavailable service mapping",
             "stale data mapping",
             "no silent success",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "error mapping policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
