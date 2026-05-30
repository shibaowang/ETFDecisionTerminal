#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto first = createDraftAndConfirm(*connection);
    const auto duplicate = invokeTradeDraftConfirm(*connection, authorizedConfirmationPayload(latestTradeDraftId(*connection)));
    bool ok = expectSuccessfulConfirmation(first);
    ok &= expectDuplicateConfirmation(duplicate);
    ok &= expect(duplicate.payload.value("duplicateHandling").toString().toStdString() == "SKIP_EXISTING_BY_DRAFT_ID",
                 "duplicate handling policy exposed");
    return ok ? 0 : 1;
}

