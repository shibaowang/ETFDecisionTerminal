#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto before = countRows(*connection, "trade_draft");
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));

    bool ok = expectSuccessfulAuditWrite(audit);
    ok &= expect(countRows(*connection, "trade_draft") == before, "audit write does not write trade_draft");
    return ok ? 0 : 1;
}
