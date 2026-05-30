#include "ShellAccountingDataServiceFactsQueryImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto querySource = readTextFile(readOnlyFactsQuerySourcePath(root));
    for (const auto& token : forbiddenWriteTokens()) {
        if (querySource.find(token) != std::string::npos) {
            std::cerr << "read-only facts query source contains forbidden write token `" << token << "`\n";
            return 1;
        }
    }
    if (countTokenInText(querySource, "SELECT ") < 5) {
        std::cerr << "read-only facts query source does not expose five SELECT-only queries\n";
        return 1;
    }
    return 0;
}
