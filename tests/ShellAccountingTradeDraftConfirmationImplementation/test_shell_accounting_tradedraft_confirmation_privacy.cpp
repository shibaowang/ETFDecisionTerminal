#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = createDraftAndConfirm(*connection);
    const auto auditPayload = latestTradeDraftAuditPayload(*connection);
    bool ok = expectSuccessfulConfirmation(call);
    const auto privacy = call.payload.value("privacy").toObject();
    ok &= expect(!privacy.value("rawSqlExposed").toBool(true), "response exposes no raw SQL");
    ok &= expect(!privacy.value("rawTradeLogPayloadExposed").toBool(true), "response exposes no raw trade_log");
    ok &= expect(!privacy.value("fullDraftPayloadExposed").toBool(true), "response exposes no full draft payload");
    ok &= expect(!containsAnyToken(auditPayload, sensitivePayloadTokens()), "audit payload is sanitized");
    return ok ? 0 : 1;
}

