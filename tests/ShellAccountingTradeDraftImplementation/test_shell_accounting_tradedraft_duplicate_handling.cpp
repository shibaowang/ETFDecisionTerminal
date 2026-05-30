#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);

    (void)invokeTradeDraftCreate(*connection);
    const auto duplicate = invokeTradeDraftCreate(*connection);
    bool ok = expectDuplicateTradeDraft(duplicate);
    ok &= expect(duplicate.payload.value("duplicateHandling").toString().toStdString()
                     == "SKIP_EXISTING_BY_DRAFT_SIGNATURE",
                 "duplicate policy documented in response");
    return ok ? 0 : 1;
}
