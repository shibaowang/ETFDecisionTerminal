#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto call = invokeTradeDraftCreate(*connection);
    bool ok = expectSuccessfulTradeDraft(call);
    ok &= expect(countRows(*connection, "audit_log") == 1, "audit event is required");
    ok &= expect(latestTradeDraftAuditPayload(*connection).find("ACCOUNTING_TRADEDRAFT_CREATE")
                     == std::string::npos,
                 "new_value payload does not duplicate action as raw confirmation");
    return ok ? 0 : 1;
}
