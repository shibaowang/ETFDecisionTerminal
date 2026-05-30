#include "ShellAccountingSnapshotRebuildWriteGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs87 = readTextFile(docs87Path(root));
    const auto docs88 = readTextFile(docs88Path(root));
    int failures = 0;

    for (const auto& token : {
             "Future snapshot rebuild / write may only be coordinated by DataService",
             "must not directly depend on DataAccess or a SQLite write path",
             "must not expose a generic write-enabled escape hatch",
             "allowlisted, statically scannable, and test-covered",
         }) {
        if (docs87.find(token) == std::string::npos) {
            std::cerr << "docs/87 missing DataService-only policy `" << token << "`\n";
            ++failures;
        }
    }
    if (docs88.find("Future rebuild only through DataService") == std::string::npos) {
        std::cerr << "docs/88 missing future rebuild only through DataService\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
