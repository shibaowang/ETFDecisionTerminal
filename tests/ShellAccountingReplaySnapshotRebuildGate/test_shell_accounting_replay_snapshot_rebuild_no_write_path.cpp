#include "ShellAccountingReplaySnapshotRebuildGate.h"

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto readOnlySource =
        dataServiceReadOnlyAccountingRegion(root) + "\n" + readTextFile(readOnlyFactsQueryPath(root));
    return containsAnyToken(readOnlySource, writeSqlForbiddenTokens()) ? 1 : 0;
}
