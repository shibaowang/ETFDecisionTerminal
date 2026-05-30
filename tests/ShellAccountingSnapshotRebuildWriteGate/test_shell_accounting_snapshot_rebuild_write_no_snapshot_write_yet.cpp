#include "ShellAccountingSnapshotRebuildWriteGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto readOnlySource =
        dataServiceReadOnlyAccountingRegion(root) + "\n" + readTextFile(readOnlyFactsQueryPath(root));
    return containsAnyToken(readOnlySource, snapshotWriteForbiddenTokens()) ? 1 : 0;
}
