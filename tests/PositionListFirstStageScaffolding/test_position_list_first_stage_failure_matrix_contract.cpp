#include "PositionListFirstStageScenario.h"

#include <iostream>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

}  // namespace

int main()
{
    const auto matrix = etfdt::tests::position_list_first_stage::firstStageFailureMatrix();
    expectTrue(!matrix.empty(), "failure matrix exists");

    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "INVALID_REQUEST"), "matrix includes INVALID_REQUEST");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "REPLAY_NOT_AVAILABLE"), "matrix includes REPLAY_NOT_AVAILABLE");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "DATA_SOURCE_MISSING"), "matrix includes DATA_SOURCE_MISSING");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "CASH_FACTS_SOURCE_UNAVAILABLE"), "matrix includes CASH_FACTS_SOURCE_UNAVAILABLE");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "POSITION_LIST_MAPPING_FAILED"), "matrix includes POSITION_LIST_MAPPING_FAILED");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "MARKET_PRICE_MISSING"), "matrix includes MARKET_PRICE_MISSING");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "MULTI_CURRENCY_UNSUPPORTED"), "matrix includes MULTI_CURRENCY_UNSUPPORTED");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "FX_RATE_MISSING"), "matrix includes FX_RATE_MISSING");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "SQLITE_READONLY_OPEN_FAILED"), "matrix includes SQLITE_READONLY_OPEN_FAILED");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "SQLITE_QUERY_FAILED"), "matrix includes SQLITE_QUERY_FAILED");
    expectTrue(etfdt::tests::position_list_first_stage::hasFailureStatus(matrix, "UNSUPPORTED_SCENARIO"), "matrix includes UNSUPPORTED_SCENARIO");

    for (const auto& entry : matrix) {
        expectTrue(entry.noWrite, entry.status + " requires no-write");
        expectTrue(entry.readOnly, entry.status + " remains readOnly=true");
        expectTrue(!entry.writeEnabled, entry.status + " remains writeEnabled=false");
        expectTrue(!entry.protocolSuccessPolicy.empty(), entry.status + " declares protocol success policy");
        expectTrue(!entry.notes.empty(), entry.status + " declares notes");
    }

    return gFailures == 0 ? 0 : 1;
}
