#include "ShellAccountingDataServiceFactsQueryImplementation.h"

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    dropPositionSnapshotForQueryError(*connection);
    return assertQueryErrorPayload(
        invokeReadOnlyAction(*connection, "position.list"),
        "position.list",
        "POSITION_LIST_QUERY_ERROR",
        "position.list query error")
        ? 0
        : 1;
}
