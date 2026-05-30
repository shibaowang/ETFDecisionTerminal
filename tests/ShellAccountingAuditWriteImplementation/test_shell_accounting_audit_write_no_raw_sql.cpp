#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));
    const auto payload = latestAuditNewValueJson(*connection) + audit.response.payloadJson;

    bool ok = expectSuccessfulAuditWrite(audit);
    ok &= expect(!containsAnyToken(payload, {"SELECT ", "INSERT ", "UPDATE ", "DELETE ", "CREATE ", "DROP ", "ALTER "}),
                 "audit payload exposes no raw SQL");
    return ok ? 0 : 1;
}
