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
    ok &= expect(payload.find("fullSnapshotPayloadExposed\":false") != std::string::npos,
                 "payload records no full snapshot exposure");
    ok &= expect(payload.find("snapshotRebuildPreview") == std::string::npos,
                 "audit payload does not mirror full preview");
    return ok ? 0 : 1;
}
