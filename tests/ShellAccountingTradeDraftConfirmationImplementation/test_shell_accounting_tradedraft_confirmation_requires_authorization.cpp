#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto create = invokeTradeDraftCreate(*connection);
    const auto draftId = latestTradeDraftId(*connection);
    const auto logBefore = countRows(*connection, "trade_log");
    const auto groupBefore = countRows(*connection, "trade_execution_group");
    const auto missing = invokeTradeDraftConfirm(
        *connection,
        authorizedConfirmationPayload(draftId).replace(
            authorizedConfirmationPayload(draftId).find(R"(,"authorization")"),
            std::string::npos,
            "}"));
    const auto wrong = invokeTradeDraftConfirm(
        *connection,
        authorizedConfirmationPayload(draftId).replace(
            authorizedConfirmationPayload(draftId).find("TASK-150_TRADEDRAFT_CONFIRM"),
            std::string("TASK-150_TRADEDRAFT_CONFIRM").size(),
            "WRONG"));
    bool ok = expect(create.response.success, "draft fixture created");
    ok &= expect(!missing.response.success, "missing authorization rejected");
    ok &= expect(!wrong.response.success, "wrong authorization rejected");
    ok &= expect(countRows(*connection, "trade_log") == logBefore,
                 "authorization failure writes no trade_log");
    ok &= expect(countRows(*connection, "trade_execution_group") == groupBefore,
                 "authorization failure writes no group");
    return ok ? 0 : 1;
}

