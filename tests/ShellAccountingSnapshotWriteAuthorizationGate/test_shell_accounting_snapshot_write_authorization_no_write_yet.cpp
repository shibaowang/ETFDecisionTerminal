#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto dataServiceSource = readTextFile(dataServiceActionsPath(root));
    const auto snapshotWriteRepository =
        readTextFile(root / "libs" / "DataAccess" / "src" / "ShellAccountingSnapshotWriteRepository.cpp");

    bool ok = dataServiceSource.find("handleAccountingSnapshotWrite") != std::string::npos;
    if (!ok) {
        return 1;
    }

    ok = snapshotWriteRepository.find("INSERT INTO cash_snapshot") != std::string::npos
        && snapshotWriteRepository.find("INSERT INTO position_snapshot") != std::string::npos
        && snapshotWriteRepository.find("INSERT INTO portfolio_summary") != std::string::npos;
    if (!ok) {
        return 1;
    }

    return containsAnyToken(snapshotWriteRepository, {
        "INSERT INTO trade_log",
        "INSERT INTO trade_execution_group",
        "INSERT INTO trade_draft",
        "INSERT INTO audit_log",
        "data.audit.append",
    }) ? 1 : 0;
}
