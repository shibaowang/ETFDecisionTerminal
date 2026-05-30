#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto before = countRows(*connection, "trade_execution_group");
    const auto call = createDraftAndConfirm(*connection);
    bool ok = expectSuccessfulConfirmation(call);
    ok &= expect(countRows(*connection, "trade_execution_group") == before + 1,
                 "confirmation writes one execution group");
    return ok ? 0 : 1;
}

