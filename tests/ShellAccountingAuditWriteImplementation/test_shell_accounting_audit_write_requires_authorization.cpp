#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    const auto missingAuth = invokeAuditWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","source":"snapshotWriteResult","sourceAction":"accounting.snapshot.write","resultStatus":"OK","snapshotWritten":true,"snapshotBatchId":"batch"})");
    const auto wrongAuth = invokeAuditWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","source":"snapshotWriteResult","sourceAction":"accounting.snapshot.write","resultStatus":"OK","snapshotWritten":true,"snapshotBatchId":"batch","authorization":"TASK-144_SNAPSHOT_WRITE"})");

    bool ok = expect(!missingAuth.response.success, "missing authorization rejected");
    ok &= expect(!wrongAuth.response.success, "wrong authorization rejected");
    ok &= expect(countRows(*connection, "audit_log") == 0, "authorization failure writes no audit row");
    return ok ? 0 : 1;
}
