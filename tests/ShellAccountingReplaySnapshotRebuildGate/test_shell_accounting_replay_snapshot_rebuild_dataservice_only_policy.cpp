#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs84 = readTextFile(docs84Path(root));
    const auto docs85 = readTextFile(docs85Path(root));
    int failures = 0;

    for (const auto& token : {
             "Future replay / rebuild may only be coordinated by DataService",
             "must not directly depend on AccountingEngine",
             "must not directly depend on AccountingEngine, DataAccess, or SQLite",
             "must not expose a write-enabled generic escape hatch",
             "allowlisted, statically scannable, and test-covered",
         }) {
        if (docs84.find(token) == std::string::npos) {
            std::cerr << "docs/84 missing DataService-only policy `" << token << "`\n";
            ++failures;
        }
    }
    if (docs85.find("Future replay only through DataService") == std::string::npos) {
        std::cerr << "docs/85 missing future replay only through DataService\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
