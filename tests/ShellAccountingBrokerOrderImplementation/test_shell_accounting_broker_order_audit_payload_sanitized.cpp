#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    (void)createConfirmedTradeAndDryRun(connection);
    const auto auditPayload = latestBrokerOrderAuditPayload(connection);
    bool ok = expect(auditPayload.find("\"dryRun\":true") != std::string::npos, "audit records dryRun");
    ok &= expect(auditPayload.find("\"brokerDisabled\":true") != std::string::npos, "audit records broker disabled");
    ok &= expect(!containsAnyToken(auditPayload, sensitivePayloadTokens()), "audit payload is sanitized");
    return ok ? 0 : 1;
}
