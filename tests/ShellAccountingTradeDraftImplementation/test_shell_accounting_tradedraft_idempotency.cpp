#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto first = invokeTradeDraftCreate(*connection);
    const auto second = invokeTradeDraftCreate(*connection);
    bool ok = expectSuccessfulTradeDraft(first);
    ok &= expectDuplicateTradeDraft(second);
    ok &= expect(countRows(*connection, "trade_draft") == 1, "idempotency keeps one draft");
    ok &= expect(countRows(*connection, "audit_log") == 1, "duplicate does not add audit row");
    return ok ? 0 : 1;
}
