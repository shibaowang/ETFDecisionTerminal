#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    const auto readOnlyRegion = dataServiceReadOnlyAccountingRegion(root);
    int failures = 0;

    if (containsAnyToken(shellBoundary, replayForbiddenTokens()) ||
        shellBoundary.find("AccountingReplayEngine") != std::string::npos) {
        ++failures;
    }
    if (containsAnyToken(readOnlyRegion, replayForbiddenTokens())) {
        ++failures;
    }
    if (readOnlyRegion.find("AccountingReplayEngine") == std::string::npos ||
        readOnlyRegion.find("calculationMode") == std::string::npos) {
        std::cerr << "authorized read-only replay must remain explicit and DataService-only\n";
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
