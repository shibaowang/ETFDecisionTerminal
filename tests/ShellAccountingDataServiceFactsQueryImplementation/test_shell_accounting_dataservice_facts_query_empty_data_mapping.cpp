#include "ShellAccountingDataServiceFactsQueryImplementation.h"

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    bool ok = true;
    ok &= assertEmptyPayload(
        invokeReadOnlyAction(*connection, "position.list"),
        "position.list",
        "POSITION_LIST_EMPTY",
        "position.list empty");
    ok &= assertEmptyPayload(
        invokeReadOnlyAction(*connection, "cash.summary"),
        "cash.summary",
        "CASH_SUMMARY_EMPTY",
        "cash.summary empty");
    ok &= assertEmptyPayload(
        invokeReadOnlyAction(*connection, "portfolio.pnl.summary"),
        "portfolio.pnl.summary",
        "PORTFOLIO_PNL_SUMMARY_EMPTY",
        "portfolio.pnl.summary empty");
    ok &= assertEmptyPayload(
        invokeReadOnlyAction(*connection, "base_position.summary"),
        "base_position.summary",
        "BASE_POSITION_SUMMARY_EMPTY",
        "base_position.summary empty");
    ok &= assertEmptyPayload(
        invokeReadOnlyAction(*connection, "sniper_pool.summary"),
        "sniper_pool.summary",
        "SNIPER_POOL_SUMMARY_EMPTY",
        "sniper_pool.summary empty");
    return ok ? 0 : 1;
}
