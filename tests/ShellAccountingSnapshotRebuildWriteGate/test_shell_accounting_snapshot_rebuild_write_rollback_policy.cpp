#include "ShellAccountingSnapshotRebuildWriteGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs87 = readTextFile(docs87Path(root));
    const auto docs88 = readTextFile(docs88Path(root));
    const auto docs86 = readTextFile(docs86Path(root));
    int failures = 0;

    for (const auto& token : {
             "Disable rebuild",
             "Disable write",
             "TASK-139 read-only replay",
             "No fake data",
             "No write fallback",
             "must not write DB",
             "must not generate TradeDraft",
             "must not add a trading fallback",
         }) {
        if (docs87.find(token) == std::string::npos && docs88.find(token) == std::string::npos) {
            std::cerr << "missing rollback token `" << token << "`\n";
            ++failures;
        }
    }
    if (docs86.find("TASK-137 read-only facts query remains the rollback and fallback path") == std::string::npos) {
        std::cerr << "docs/86 must preserve read-only replay fallback wording\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
