#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto logBefore = countRows(*connection, "trade_log");
    const auto missing = invokeTradeDraftConfirm(*connection, authorizedConfirmationPayload(9999));
    bool ok = expect(!missing.response.success, "missing draft rejected");
    ok &= expect(countRows(*connection, "trade_log") == logBefore, "missing draft writes no trade_log");
    return ok ? 0 : 1;
}

