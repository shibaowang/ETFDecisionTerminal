#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    const auto missingSource = invokeAuditWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","sourceAction":"accounting.snapshot.write","resultStatus":"OK","snapshotWritten":true,"snapshotBatchId":"batch","authorization":"TASK-146_AUDIT_WRITE"})");
    const auto failedSource = invokeAuditWrite(
        *connection,
        R"({"accountId":"1001","portfolioId":"1001","source":"snapshotWriteResult","sourceAction":"accounting.snapshot.write","resultStatus":"FAILED","snapshotWritten":false,"snapshotBatchId":"batch","authorization":"TASK-146_AUDIT_WRITE"})");

    bool ok = expect(!missingSource.response.success, "missing snapshotWriteResult source rejected");
    ok &= expect(failedSource.response.success, "failed source maps to structured response");
    ok &= expect(failedSource.payload.value("status").toString().toStdString() == "SOURCE_WRITE_FAILED",
                 "failed source status recorded");
    ok &= expect(countRows(*connection, "audit_log") == 0, "no audit row without snapshot write result");
    return ok ? 0 : 1;
}
