#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto call = invokeTradeDraftCreate(*connection);
    const auto privacy = call.payload.value("privacy").toObject();
    bool ok = expectSuccessfulTradeDraft(call);
    ok &= expect(!privacy.value("rawSqlExposed").toBool(true), "response exposes no raw SQL");
    ok &= expect(!privacy.value("rawTradeLogPayloadExposed").toBool(true), "response exposes no raw trade_log");
    ok &= expect(!privacy.value("fullSnapshotPayloadExposed").toBool(true), "response exposes no full snapshot");
    ok &= expect(!privacy.value("internalStackExposed").toBool(true), "response exposes no stack trace");
    ok &= expect(!containsAnyToken(latestTradeDraftAuditPayload(*connection), sensitivePayloadTokens()),
                 "audit payload privacy tokens absent");
    return ok ? 0 : 1;
}
