#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto before = countRows(*connection, "trade_log");
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));

    bool ok = expectSuccessfulAuditWrite(audit);
    ok &= expect(countRows(*connection, "trade_log") == before, "audit write does not write trade_log");
    return ok ? 0 : 1;
}
