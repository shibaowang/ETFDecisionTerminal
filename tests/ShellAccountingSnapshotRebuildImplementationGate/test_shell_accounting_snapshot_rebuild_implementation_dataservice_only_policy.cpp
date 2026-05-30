#include "ShellAccountingSnapshotRebuildImplementationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs89 = readTextFile(docs89Path(root));
    const auto docs90 = readTextFile(docs90Path(root));
    int failures = 0;

    for (const auto& token : {
             "Future snapshot rebuild may only run inside the DataService boundary",
             "must not directly depend on",
             "AccountingEngine rebuild helpers",
             "must be allowlisted",
             "must not expose a generic write-enabled escape hatch",
         }) {
        if (docs89.find(token) == std::string::npos) {
            std::cerr << "docs/89 missing DataService-only policy `" << token << "`\n";
            ++failures;
        }
    }
    if (docs90.find("Future rebuild runs only through DataService") == std::string::npos) {
        std::cerr << "docs/90 missing DataService-only rebuild matrix row\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
