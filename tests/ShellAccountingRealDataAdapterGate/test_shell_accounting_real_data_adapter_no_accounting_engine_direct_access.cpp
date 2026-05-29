#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto files = realAdapterBoundaryFiles(root);
    for (const auto& token : accountingEngineDirectTokens()) {
        if (containsTokenInFiles(files, token)) {
            std::cerr << "AccountingEngine direct access token found: `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
