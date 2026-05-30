#include "ShellAccountingSnapshotRebuildWriteGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    return containsAnyToken(shellBoundary, uiTriggeredWriteForbiddenTokens()) ? 1 : 0;
}
