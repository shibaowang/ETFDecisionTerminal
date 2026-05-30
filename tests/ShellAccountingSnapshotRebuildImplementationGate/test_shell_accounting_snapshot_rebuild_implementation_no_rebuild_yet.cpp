#include "ShellAccountingSnapshotRebuildImplementationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    const auto readOnlyRegion = dataServiceReadOnlyAccountingRegion(root);
    int failures = 0;
    if (readOnlyRegion.find("snapshotRebuildPreview") == std::string::npos) {
        ++failures;
    }
    if (containsAnyToken(shellBoundary + "\n" + readOnlyRegion, rebuildForbiddenTokens())) {
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
