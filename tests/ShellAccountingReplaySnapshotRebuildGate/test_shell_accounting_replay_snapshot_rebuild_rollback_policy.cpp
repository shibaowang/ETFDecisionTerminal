#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs84 = readTextFile(docs84Path(root));
    const auto docs85 = readTextFile(docs85Path(root));
    const auto docs83 = readTextFile(docs83Path(root));
    int failures = 0;

    for (const auto& token : {
             "Disabling replay",
             "Disabling rebuild",
             "TASK-137 read-only facts query",
             "must not fake",
             "must not write DB",
             "must not generate trade suggestions",
             "must not generate TradeDraft",
             "must not add a trading fallback",
         }) {
        if (docs84.find(token) == std::string::npos && docs85.find(token) == std::string::npos) {
            std::cerr << "missing rollback token `" << token << "`\n";
            ++failures;
        }
    }
    if (docs83.find("TASK-137 facts query remains a read-only fallback") == std::string::npos) {
        std::cerr << "docs/83 must preserve TASK-137 fallback wording\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
