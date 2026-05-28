#include "PositionListMappingContractTestUtils.h"

#include "AccountingEngine/AccountingIssue.h"
#include "AccountingEngine/AccountingReplayValidation.h"

#include <iostream>

namespace {

int failures = 0;

void require(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        ++failures;
    }
}

bool hasOutput(const etfdt::accounting::ReplayRequestDto& request, const std::string& output)
{
    for (const auto& item : request.requestedOutputs) {
        if (item == output) {
            return true;
        }
    }
    return false;
}

}  // namespace

int main()
{
    using namespace etfdt::tests::position_list_mapping;

    const std::string payload = R"({
        "accountId": "ACC-DEMO-001",
        "portfolioId": "PF-DEMO-001",
        "sourceFromTime": "2026-01-01T00:00:00Z",
        "sourceToTime": "2026-01-31T23:59:59Z",
        "includeIssues": true,
        "requestedOutputs": ["positions", "issues"],
        "calculationVersion": "accounting-replay-v0"
    })";

    const auto mapped = mapPositionListRequestToReplayRequestForTest(payload);
    require(mapped.success, "valid object payload maps successfully");
    require(mapped.value.accountId == "ACC-DEMO-001", "accountId maps to ReplayRequestDto");
    require(mapped.value.portfolioId == "PF-DEMO-001", "portfolioId maps to ReplayRequestDto");
    require(mapped.value.sourceFromTime == "2026-01-01T00:00:00Z", "sourceFromTime maps to ReplayRequestDto");
    require(mapped.value.sourceToTime == "2026-01-31T23:59:59Z", "sourceToTime maps to ReplayRequestDto");
    require(mapped.value.includeIssues, "includeIssues maps to ReplayRequestDto");
    require(hasOutput(mapped.value, etfdt::accounting::RequestedReplayOutput::Positions),
            "requestedOutputs includes positions");
    require(hasOutput(mapped.value, etfdt::accounting::RequestedReplayOutput::Issues),
            "requestedOutputs includes issues");
    require(mapped.value.calculationVersion == "accounting-replay-v0",
            "calculationVersion maps to ReplayRequestDto");

    const auto noAccount = mapPositionListRequestToReplayRequestForTest(R"({"portfolioId":"PF-DEMO-001"})");
    require(noAccount.success, "missing accountId remains a valid mapping");
    require(noAccount.value.accountId.empty(), "missing accountId maps to empty string");

    const auto noPortfolio = mapPositionListRequestToReplayRequestForTest(R"({"accountId":"ACC-DEMO-001"})");
    require(noPortfolio.success, "missing portfolioId remains a valid mapping");
    require(noPortfolio.value.portfolioId.empty(), "missing portfolioId maps to empty string");

    const auto nonObject = mapPositionListRequestToReplayRequestForTest(R"(["positions"])");
    require(!nonObject.success, "non-object payload is a controlled parse error");
    require(nonObject.errorCode == "INVALID_REQUEST", "non-object payload returns INVALID_REQUEST");

    const auto invalidRange = mapPositionListRequestToReplayRequestForTest(R"({
        "sourceFromTime": "2026-02-01T00:00:00Z",
        "sourceToTime": "2026-01-01T00:00:00Z",
        "requestedOutputs": ["positions"]
    })");
    require(invalidRange.success, "invalid time range still maps to ReplayRequestDto");
    const auto validation = etfdt::accounting::validateReplayRequest(invalidRange.value);
    require(!validation.valid, "invalid time range is caught by validateReplayRequest");
    require(validation.hasIssueCode(etfdt::accounting::AccountingIssueCode::InvalidTimeRange),
            "invalid time range reports INVALID_TIME_RANGE");

    return failures == 0 ? 0 : 1;
}
