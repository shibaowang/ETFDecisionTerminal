#include "ShellAccountingBrokerOrderImplementation.h"

using namespace etfdt::tests::shell_accounting_broker_order;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(connection);
    (void)createDraftAndConfirm(connection);
    auto call = invokeBrokerOrderDryRun(
        connection,
        std::string(R"({"draftId":)") + std::to_string(latestTradeDraftId(connection))
            + R"(,"accountId":"1001","portfolioId":"1001","source":"confirmedTradeDraft","userConfirmed":true})");
    return expect(!call.response.success, "missing authorization fails") ? 0 : 1;
}
