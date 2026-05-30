#include "ShellAccountingAuditWriteAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto snapshotAction = snapshotWriteActionRegion(root);
    const auto snapshotRepository = readTextFile(snapshotWriteRepositoryPath(root));

    if (snapshotAction.find("handleAccountingSnapshotWrite") == std::string::npos) {
        return 1;
    }
    if (snapshotRepository.find("INSERT INTO cash_snapshot") == std::string::npos) {
        return 1;
    }

    return containsAnyToken(snapshotAction + snapshotRepository, auditWriteForbiddenTokens()) ? 1 : 0;
}
