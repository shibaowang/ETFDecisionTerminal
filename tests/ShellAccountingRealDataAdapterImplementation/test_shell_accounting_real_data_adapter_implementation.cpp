#include "ShellAccountingRealDataAdapterImplementation.h"

#include <filesystem>
#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs80 = readTextFile(docs80Path(root));
    const auto startup = productionStartupFiles(root);

    if (docs80.find("TASK-135") == std::string::npos ||
        docs80.find("read-only adapter") == std::string::npos ||
        docs80.find("not a write path") == std::string::npos ||
        docs80.find("TradeDraft") == std::string::npos) {
        std::cerr << "docs/80 must record TASK-135 read-only adapter scope\n";
        return 1;
    }
    if (countTokenInFiles(startup, "ShellAccountingDataServiceAdapter") == 0 ||
        countTokenInFiles(startup, "ShellAccountingDataServiceClientPortAdapter") == 0 ||
        countTokenInFiles(startup, "DataServiceClient") == 0 ||
        countTokenInFiles(startup, "setServiceAdapter(") != 1) {
        std::cerr << "production startup must wire exactly one authorized read-only adapter chain\n";
        return 1;
    }
    return 0;
}
