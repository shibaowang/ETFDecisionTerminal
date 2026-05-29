#include "ShellAccountingDataServiceFactsQueryGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    if (!std::filesystem::exists(docs81Path(root)) || !std::filesystem::exists(docs82Path(root))) {
        std::cerr << "docs/81 and docs/82 must exist\n";
        return 1;
    }

    const auto docs = readTextFile(docs81Path(root)) + "\n" + readTextFile(docs82Path(root));
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
             "TASK-134",
             "TASK-135",
             "user explicitly authorized DataService facts query implementation",
             "facts query is not write path",
             "facts query is not AccountingEngine replay",
             "facts query is not snapshot rebuild",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "missing DataService facts query gate token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
