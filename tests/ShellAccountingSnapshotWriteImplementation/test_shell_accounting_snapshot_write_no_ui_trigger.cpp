#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    const auto shellText = shellBoundaryText(sourceRoot(argc, argv));
    return containsAnyToken(shellText, {
        "accounting.snapshot.write",
        "ShellAccountingSnapshotWriteRepository",
        "INSERT INTO cash_snapshot",
        "INSERT INTO position_snapshot",
        "INSERT INTO portfolio_summary",
        "DataAccess/ShellAccountingSnapshotWriteRepository",
    }) ? 1 : 0;
}
