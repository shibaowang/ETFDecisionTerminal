#include "ShellAccountingDataServiceFactsQueryImplementation.h"

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReadOnlyFactsFixture(*connection);
    return assertReadOnlySuccessPayload(
        invokeReadOnlyAction(*connection, "sniper_pool.summary"),
        "sniper_pool.summary",
        "tierSummary",
        "sniper_pool.summary")
        ? 0
        : 1;
}
