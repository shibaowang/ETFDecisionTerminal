#include "ShellAccountingDataServiceFactsQueryImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto source = readTextFile(dataServiceActionsPath(root))
        + readTextFile(readOnlyFactsQuerySourcePath(root));
    for (const auto& token : forbiddenReplayTokens()) {
        if (source.find(token) != std::string::npos) {
            std::cerr << "facts query implementation contains replay token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
