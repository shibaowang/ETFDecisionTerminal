#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    const auto invalid = invokeAuditWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","source":"snapshotWriteResult","sourceAction":"accounting.snapshot.write","resultStatus":"OK","snapshotWritten":true,"authorization":"TASK-146_AUDIT_WRITE"})");

    bool ok = expect(!invalid.response.success, "missing batch id rejects audit write");
    ok &= expect(countRows(*connection, "audit_log") == 0, "invalid audit write rolls back/no row");
    return ok ? 0 : 1;
}
