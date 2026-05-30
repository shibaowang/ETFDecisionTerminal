#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellText = shellBoundaryText(root);
    const auto actionsText = dataServiceActionsText(root);

    bool ok = expect(shellText.find("accounting.snapshot.write") == std::string::npos,
                     "Shell/QML does not call accounting.snapshot.write");
    ok &= expect(shellText.find("ShellAccountingSnapshotWriteRepository") == std::string::npos,
                 "Shell/QML does not depend on snapshot write repository");
    ok &= expect(actionsText.find("handleAccountingSnapshotWrite") != std::string::npos,
                 "snapshot write is implemented inside DataService action boundary");
    ok &= expect(dataAccessSnapshotWriteText(root).find("INSERT INTO cash_snapshot") != std::string::npos,
                 "allowlist write SQL is isolated in DataAccess repository");
    return ok ? 0 : 1;
}
