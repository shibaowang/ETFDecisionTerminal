#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));
    const auto privacy = audit.payload.value("privacy").toObject();

    bool ok = expectSuccessfulAuditWrite(audit);
    ok &= expect(!privacy.value("rawSqlExposed").toBool(true), "rawSqlExposed false");
    ok &= expect(!privacy.value("rawTradeLogPayloadExposed").toBool(true), "rawTradeLogPayloadExposed false");
    ok &= expect(!privacy.value("fullSnapshotPayloadExposed").toBool(true), "fullSnapshotPayloadExposed false");
    ok &= expect(!privacy.value("internalStackExposed").toBool(true), "internalStackExposed false");
    return ok ? 0 : 1;
}
