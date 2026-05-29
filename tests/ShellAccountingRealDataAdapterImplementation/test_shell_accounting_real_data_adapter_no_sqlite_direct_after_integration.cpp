#include "ShellAccountingRealDataAdapterImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_real_data_adapter_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    auto files = adapterBoundaryFiles(root);
    const auto presenterFiles = presenterCoreFiles(root);
    const auto qmlFiles = productionQmlFiles(root);
    files.insert(files.end(), presenterFiles.begin(), presenterFiles.end());
    files.insert(files.end(), qmlFiles.begin(), qmlFiles.end());

    for (const auto& token : sqliteDirectTokens()) {
        if (containsTokenInFiles(files, token)) {
            std::cerr << "ShellAccounting adapter/presenter/QML path directly references SQLite/DataAccess token `"
                      << token << "`\n";
            return 1;
        }
    }
    return 0;
}
