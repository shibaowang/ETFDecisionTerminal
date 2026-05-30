#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto create = invokeTradeDraftCreate(*connection);
    const auto logBefore = countRows(*connection, "trade_log");
    auto payload = authorizedConfirmationPayload(latestTradeDraftId(*connection));
    payload.replace(payload.find(R"("quantity1e6":1000000)"), std::string(R"("quantity1e6":1000000)").size(), R"("quantity1e6":2000000)");
    const auto invalid = invokeTradeDraftConfirm(*connection, payload);
    bool ok = expect(create.response.success, "draft fixture created");
    ok &= expect(!invalid.response.success, "invalid draft payload rejected");
    ok &= expect(countRows(*connection, "trade_log") == logBefore, "invalid payload writes no trade_log");
    return ok ? 0 : 1;
}

