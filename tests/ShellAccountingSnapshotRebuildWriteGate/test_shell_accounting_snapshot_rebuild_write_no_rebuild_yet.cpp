#include "ShellAccountingSnapshotRebuildWriteGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    const auto readOnlyRegion = dataServiceReadOnlyAccountingRegion(root);
    int failures = 0;

    if (containsAnyToken(shellBoundary, rebuildForbiddenTokens())) {
        ++failures;
    }
    if (containsAnyToken(readOnlyRegion, rebuildForbiddenTokens())) {
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
