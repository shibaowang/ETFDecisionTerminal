#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = createDraftAndConfirm(*connection);
    const auto auditPayload = latestTradeDraftAuditPayload(*connection);
    bool ok = expectSuccessfulConfirmation(call);
    ok &= expect(auditPayload.find("accounting.tradedraft.confirm") != std::string::npos,
                 "confirmation audit source action recorded");
    ok &= expect(auditPayload.find("TASK-150_TRADEDRAFT_CONFIRM") != std::string::npos,
                 "confirmation audit authorization recorded");
    return ok ? 0 : 1;
}

