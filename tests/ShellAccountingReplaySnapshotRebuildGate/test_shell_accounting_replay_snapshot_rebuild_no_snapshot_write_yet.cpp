#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto readOnlySource =
        dataServiceReadOnlyAccountingRegion(root) + "\n" + readTextFile(readOnlyFactsQueryPath(root));
    if (containsAnyToken(readOnlySource, snapshotWriteForbiddenTokens())) {
        return 1;
    }
    return 0;
}
