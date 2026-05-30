#include "ShellAccountingReplaySnapshotRebuildGate.h"

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    return containsAnyToken(shellBoundary, uiDirectForbiddenTokens()) ? 1 : 0;
}
