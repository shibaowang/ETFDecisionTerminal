#include "ShellAccountingDataServiceFactsQueryImplementation.h"

using namespace etfdt::tests::shell_accounting_dataservice_facts_query_implementation;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReadOnlyFactsFixture(*connection);
    const auto payload = invokeReadOnlyAction(
        *connection,
        "position.list",
        "{\"accountId\":\"999\",\"portfolioId\":\"999\"}");
    return assertEmptyPayload(payload, "position.list", "POSITION_LIST_EMPTY", "position.list missing filter")
        ? 0
        : 1;
}
