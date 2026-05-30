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
             "Snapshot rebuild input may only come from TASK-139 read-only replay",
             "must not use fake trade facts",
             "If read-only replay is unavailable, stale, incomplete, or inconsistent",
             "silently succeed",
         }) {
        if (docs89.find(token) == std::string::npos) {
            std::cerr << "docs/89 missing read-only replay input policy `" << token << "`\n";
            ++failures;
        }
    }
    if (docs90.find("Future rebuild input comes from TASK-139 read-only replay") == std::string::npos) {
        std::cerr << "docs/90 missing read-only replay input matrix row\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
