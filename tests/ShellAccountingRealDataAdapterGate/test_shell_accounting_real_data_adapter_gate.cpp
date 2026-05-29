#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs78 = readTextFile(docs78Path(root));
    const auto docs79 = readTextFile(docs79Path(root));
    for (const auto& token : {
             "TASK-119",
             "TASK-120",
             "TASK-121",
             "TASK-122",
             "TASK-123",
             "TASK-124",
             "TASK-125",
             "TASK-126",
             "TASK-127",
             "TASK-128",
             "TASK-129",
             "TASK-130",
             "TASK-131",
             "TASK-132",
             "TASK-133",
         }) {
        if (docs78.find(token) == std::string::npos && docs79.find(token) == std::string::npos) {
            std::cerr << "missing pre-gate token `" << token << "`\n";
            return 1;
        }
    }
    for (const auto& token : {
             "docs/78 merged",
             "docs/79 merged",
             "user explicitly authorized real data adapter implementation",
             "real adapter is not write path",
             "read-only DataService action is not TradeDraft",
         }) {
        if (docs78.find(token) == std::string::npos && docs79.find(token) == std::string::npos) {
            std::cerr << "missing gate token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
