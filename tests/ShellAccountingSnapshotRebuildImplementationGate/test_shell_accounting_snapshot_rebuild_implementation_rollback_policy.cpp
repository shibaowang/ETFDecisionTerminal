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
             "Disabling rebuild must fall back to TASK-139 read-only replay",
             "Fallback must not fake data",
             "must not write database tables",
             "must not generate TradeDraft",
             "must not generate trade suggestions",
             "must not execute strategy",
             "must not bypass DataService",
             "Snapshot write remains separately",
         }) {
        if (docs89.find(token) == std::string::npos && docs90.find(token) == std::string::npos) {
            std::cerr << "missing rollback token `" << token << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
