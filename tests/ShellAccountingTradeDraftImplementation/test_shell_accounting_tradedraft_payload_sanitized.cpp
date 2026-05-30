#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto call = invokeTradeDraftCreate(*connection);
    const auto auditPayload = latestTradeDraftAuditPayload(*connection);
    bool ok = expectSuccessfulTradeDraft(call);
    ok &= expect(auditPayload.find("sourceAction") != std::string::npos, "audit payload has source action");
    ok &= expect(auditPayload.find("rawSqlExposed\":false") != std::string::npos, "raw SQL redacted");
    ok &= expect(auditPayload.find("fullSnapshotPayloadExposed\":false") != std::string::npos,
                 "full snapshot redacted");
    ok &= expect(!containsAnyToken(auditPayload, sensitivePayloadTokens()), "no sensitive token in audit payload");
    return ok ? 0 : 1;
}
