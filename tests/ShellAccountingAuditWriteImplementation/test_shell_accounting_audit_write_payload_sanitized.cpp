#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto snapshot = invokeSnapshotWrite(*connection);
    const auto audit = invokeAuditWrite(*connection, auditWritePayloadFromSnapshot(snapshot));
    const auto payload = latestAuditNewValueJson(*connection);

    bool ok = expectSuccessfulAuditWrite(audit);
    ok &= expect(payload.find("sourceAction") != std::string::npos, "payload has action name");
    ok &= expect(payload.find("snapshotBatchId") != std::string::npos, "payload has batch id");
    ok &= expect(payload.find("sanitizedIssueCode") != std::string::npos, "payload has sanitized issue code");
    ok &= expect(payload.find("\"sanitized\":true") != std::string::npos
                     || audit.payload.value("payloadPolicy").toObject().value("sanitized").toBool(false),
                 "response marks payload sanitized");
    return ok ? 0 : 1;
}
