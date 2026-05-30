#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto create = invokeTradeDraftCreate(*connection);
    const auto logBefore = countRows(*connection, "trade_log");
    auto payload = authorizedConfirmationPayload(latestTradeDraftId(*connection));
    payload.insert(payload.size() - 1, R"(,"confirmationDisabled":true)");
    const auto disabled = invokeTradeDraftConfirm(*connection, payload);
    bool ok = expect(create.response.success, "draft fixture created");
    ok &= expect(!disabled.response.success, "disabled confirmation rejected");
    ok &= expect(countRows(*connection, "trade_log") == logBefore, "disabled writes no trade_log");
    return ok ? 0 : 1;
}

