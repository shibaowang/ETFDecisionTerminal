#include "ShellAccountingSnapshotRebuildImplementationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto scannedText =
        readTextFile(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
        + "\n" + dataServiceReadOnlyAccountingRegion(root);
    return containsAnyToken(scannedText, {
        "brokerOrder(",
        "placeOrder",
        "\"strategyExecute\"",
        "executeStrategy(",
        "buyButton",
        "sellButton",
        "orderConfirmation",
    }) ? 1 : 0;
}
