#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    const auto call = invokeTradeDraftCreate(*connection);
    bool ok = expectSuccessfulTradeDraft(call);
    ok &= expect(latestTradeDraftAction(*connection) == "BUY", "draft side persisted");
    ok &= expect(!latestTradeDraftUid(*connection).empty(), "draft uid persisted");
    return ok ? 0 : 1;
}
