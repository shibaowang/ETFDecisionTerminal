#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto before = countRows(*connection, "trade_log");
    const auto call = createDraftAndConfirm(*connection);
    bool ok = expectSuccessfulConfirmation(call);
    ok &= expect(countRows(*connection, "trade_log") == before + 1, "confirmation writes one trade_log row");
    return ok ? 0 : 1;
}

