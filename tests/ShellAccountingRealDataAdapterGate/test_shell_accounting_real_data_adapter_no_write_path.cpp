#include "ShellAccountingRealDataAdapterGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    auto files = productionStartupFiles(root);
    const auto qmlFiles = productionQmlFiles(root);
    const auto presenterFiles = presenterCoreFiles(root);
    files.insert(files.end(), qmlFiles.begin(), qmlFiles.end());
    files.push_back(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    files.push_back(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h");

    for (const auto& token : writePathTokens()) {
        if (containsTokenInFiles(files, token)) {
            std::cerr << "write path token found before real adapter task: `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
