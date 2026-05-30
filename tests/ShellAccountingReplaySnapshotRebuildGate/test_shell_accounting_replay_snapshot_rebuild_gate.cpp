#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs84 = readTextFile(docs84Path(root));
    const auto docs85 = readTextFile(docs85Path(root));
    int failures = 0;

    if (!std::filesystem::exists(docs84Path(root)) || !std::filesystem::exists(docs85Path(root))) {
        std::cerr << "docs/84 and docs/85 must exist\n";
        ++failures;
    }
    for (const auto& task : taskPrerequisiteTokens()) {
        if (docs84.find(task) == std::string::npos) {
            std::cerr << "docs/84 missing prerequisite " << task << "\n";
            ++failures;
        }
    }
    for (const auto& token : {
             "explicit user authorization",
             "Replay is not production QML binding",
             "Replay is not TradeDraft or trading suggestion",
             "Snapshot rebuild is not trade execution",
             "Snapshot write, if ever needed, must be explicitly authorized",
         }) {
        if (docs84.find(token) == std::string::npos) {
            std::cerr << "docs/84 missing `" << token << "`\n";
            ++failures;
        }
    }
    if (docs85.find("Test Matrix") == std::string::npos) {
        std::cerr << "docs/85 missing Test Matrix\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
