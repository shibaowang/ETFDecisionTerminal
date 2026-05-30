#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    const auto readOnlyRegion = dataServiceReadOnlyAccountingRegion(root);
    int failures = 0;

    if (containsAnyToken(shellBoundary, replayForbiddenTokens())) {
        ++failures;
    }
    if (containsAnyToken(readOnlyRegion, replayForbiddenTokens())) {
        ++failures;
    }
    for (const auto& forbiddenAction : {
             "accounting.replay.snapshot",
             "accounting.snapshot.rebuild",
             "accounting.replay.execute",
         }) {
        if (readOnlyRegion.find(forbiddenAction) != std::string::npos) {
            std::cerr << "unexpected replay/rebuild DataService action `" << forbiddenAction << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
