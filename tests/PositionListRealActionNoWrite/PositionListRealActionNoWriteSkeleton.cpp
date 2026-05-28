#include "PositionListRealActionNoWriteSkeleton.h"

#include <algorithm>

namespace etfdt::tests::position_list_no_write {

std::vector<PositionListRealActionNoWriteScenario> positionListRealActionNoWriteScenarios()
{
    return {
        {
            "valid_request_guard_path",
            R"({"accountId":"ACC-DEMO-001","portfolioId":"PF-DEMO-001"})",
            "POSITION_LIST_NOT_AVAILABLE",
            false,
            true,
            false,
            true,
            "Current skeleton uses the guard-shaped path and must keep no-write coverage.",
        },
        {
            "invalid_request_protocol_error_path",
            R"([])",
            "INVALID_REQUEST",
            false,
            true,
            false,
            true,
            "Future malformed or non-object request handling must remain no-write.",
        },
        {
            "replay_unavailable_path",
            R"({"accountId":"ACC-DEMO-001"})",
            "REPLAY_NOT_AVAILABLE",
            false,
            true,
            false,
            true,
            "Future AccountingEngine unavailable path must not write tables.",
        },
        {
            "facts_query_unavailable_path",
            R"({"accountId":"ACC-DEMO-001"})",
            "DATA_SOURCE_MISSING",
            false,
            true,
            false,
            true,
            "Future DataAccess facts query unavailable path must not write tables.",
        },
        {
            "mapping_failure_path",
            R"({"accountId":"ACC-DEMO-001"})",
            "POSITION_LIST_MAPPING_FAILED",
            false,
            true,
            false,
            true,
            "Future TradeFactRow to TradeFactDto mapping failure path must remain no-write.",
        },
        {
            "missing_market_price_path",
            R"({"accountId":"ACC-DEMO-001","requestedOutputs":["positions"]})",
            "MARKET_PRICE_MISSING",
            false,
            true,
            false,
            true,
            "Future missing market price path must not fake valuation and must not write.",
        },
        {
            "multi_currency_unsupported_path",
            R"({"accountId":"ACC-DEMO-001","requestedOutputs":["positions"]})",
            "MULTI_CURRENCY_UNSUPPORTED",
            false,
            true,
            false,
            true,
            "Future multi-currency unsupported path must not fake FX conversion and must not write.",
        },
        {
            "fx_rate_missing_path",
            R"({"accountId":"ACC-DEMO-001","requestedOutputs":["positions"]})",
            "FX_RATE_MISSING",
            false,
            true,
            false,
            true,
            "Future missing FX rate path must remain no-write.",
        },
        {
            "sqlite_readonly_open_failed_path",
            R"({"accountId":"ACC-DEMO-001"})",
            "SQLITE_READONLY_OPEN_FAILED",
            false,
            true,
            false,
            true,
            "Future read-only open failure path must remain no-write.",
        },
        {
            "sqlite_query_failed_path",
            R"({"accountId":"ACC-DEMO-001"})",
            "SQLITE_QUERY_FAILED",
            false,
            true,
            false,
            true,
            "Future SELECT query failure path must remain no-write.",
        },
    };
}

std::vector<PositionListRealActionNoWriteScenario>
positionListRealActionFailureNoWriteScenarios()
{
    auto scenarios = positionListRealActionNoWriteScenarios();
    scenarios.erase(
        std::remove_if(
            scenarios.begin(),
            scenarios.end(),
            [](const PositionListRealActionNoWriteScenario& scenario) {
                return scenario.scenarioName == "valid_request_guard_path";
            }),
        scenarios.end());
    return scenarios;
}

bool hasScenario(
    const std::vector<PositionListRealActionNoWriteScenario>& scenarios,
    const std::string& scenarioName)
{
    return std::any_of(
        scenarios.begin(),
        scenarios.end(),
        [&](const PositionListRealActionNoWriteScenario& scenario) {
            return scenario.scenarioName == scenarioName;
        });
}

FakePositionListActionResult runFakePositionListRealActionCallbackForTest(
    const PositionListRealActionNoWriteScenario& scenario)
{
    return {
        scenario.scenarioName,
        scenario.expectedDomainStatus,
        scenario.expectedImplemented,
        scenario.expectedReadOnly,
        scenario.expectedWriteEnabled,
    };
}

}  // namespace etfdt::tests::position_list_no_write
