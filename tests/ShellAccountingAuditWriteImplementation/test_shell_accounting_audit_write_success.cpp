#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));

    bool ok = expectSuccessfulAuditWrite(audit);
    ok &= expect(countRows(*connection, "audit_log") == 1, "audit_log row count");
    ok &= expect(latestAuditNewValueJson(*connection).find("TASK-146_AUDIT_WRITE") != std::string::npos,
                 "audit payload records authorization token");
    return ok ? 0 : 1;
}
