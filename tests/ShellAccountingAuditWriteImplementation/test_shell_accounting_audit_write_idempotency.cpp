#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto payload = auditWritePayloadFromSnapshot(snapshot);
    const auto first = invokeAuditWrite(*connection, payload);
    const auto second = invokeAuditWrite(*connection, payload);

    bool ok = expectSuccessfulAuditWrite(first);
    ok &= expectDuplicateAuditWrite(second);
    ok &= expect(countRows(*connection, "audit_log") == 1, "idempotency keeps one audit row");
    return ok ? 0 : 1;
}
