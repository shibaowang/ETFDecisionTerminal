#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto scannedText =
        readTextFile(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
        + "\n" + dataServiceReadOnlyAccountingRegion(root);
    return containsAnyToken(scannedText, tradeOrStrategyForbiddenTokens()) ? 1 : 0;
}
