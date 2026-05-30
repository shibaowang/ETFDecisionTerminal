#include "ShellAccountingSnapshotRebuildImplementationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    const auto readOnlyRegion = dataServiceReadOnlyAccountingRegion(root);
    return containsAnyToken(shellBoundary + "\n" + readOnlyRegion, rebuildForbiddenTokens()) ? 1 : 0;
}
