#include "ShellAccountingSnapshotRebuildImplementationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto scannedText =
        dataServiceReadOnlyAccountingRegion(root) + "\n" + readTextFile(readOnlyFactsQueryPath(root));
    return containsAnyToken(scannedText, writeForbiddenTokens()) ? 1 : 0;
}
