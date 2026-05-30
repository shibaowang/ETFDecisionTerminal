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
    ok &= expect(payload.find("rawTradeLogPayloadExposed\":false") != std::string::npos,
                 "payload records no raw trade log exposure");
    ok &= expect(payload.find("tradeFacts") == std::string::npos
                     && payload.find("trade_log payload") == std::string::npos,
                 "payload contains no raw trade fact list");
    return ok ? 0 : 1;
}
