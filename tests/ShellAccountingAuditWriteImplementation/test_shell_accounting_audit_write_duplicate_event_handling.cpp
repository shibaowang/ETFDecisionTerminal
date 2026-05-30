#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto payload = auditWritePayloadFromSnapshot(snapshot);
    (void)invokeAuditWrite(*connection, payload);
    const auto duplicate = invokeAuditWrite(*connection, payload);

    bool ok = expectDuplicateAuditWrite(duplicate);
    ok &= expect(duplicate.payload.value("duplicateHandling").toString().toStdString()
                     == "SKIP_EXISTING_BY_SOURCE_RESULT",
                 "duplicate handling policy recorded");
    return ok ? 0 : 1;
}
