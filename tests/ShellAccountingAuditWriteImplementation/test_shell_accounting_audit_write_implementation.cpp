#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));

    bool ok = expectSuccessfulSnapshotWrite(snapshot);
    ok &= expectSuccessfulAuditWrite(audit);
    ok &= expect(countRows(*connection, "audit_log") == 1, "one audit_log row is written");
    ok &= expect(docs97Text(root).find("TASK-146") != std::string::npos, "docs/97 records TASK-146");
    return ok ? 0 : 1;
}
