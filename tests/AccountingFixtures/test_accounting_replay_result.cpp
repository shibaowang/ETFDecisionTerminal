#include "AccountingReplayResult.h"
#include "AccountingReplayResultMapper.h"
#include "AccountingReplayStubEngine.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

using etfdt::tests::accounting::AccountingFixture;
using etfdt::tests::accounting::AccountingReplayResultMapper;
using etfdt::tests::accounting::AccountingReplayStubEngine;
using etfdt::tests::accounting::hasEmptyReplayOutputs;
using etfdt::tests::accounting::hasReplayIssueCode;
using etfdt::tests::accounting::kReplayStatusInvalidFixture;
using etfdt::tests::accounting::kReplayStatusNotImplemented;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(1);
    }
}

AccountingFixture fixture(
    std::string fixtureId,
    std::string schemaVersion,
    int issueCount,
    bool blocking)
{
    AccountingFixture value;
    value.fixtureId = std::move(fixtureId);
    value.schemaVersion = std::move(schemaVersion);
    value.blocking = blocking;
    for (int i = 0; i < issueCount; ++i) {
        value.expectedIssues.push_back({"ERROR", "FIXTURE_EXPECTED_ISSUE", "Expected fixture issue.", true});
    }
    return value;
}

} // namespace

int main()
{
    const auto fx001 = fixture("FX001_EMPTY_LEDGER", "0.1", 0, false);
    const auto fx004 = fixture("FX004_SELL_EXCEEDS_POSITION", "0.1", 1, true);

    const auto notImplemented = AccountingReplayResultMapper::makeNotImplementedResult(fx004);
    require(!notImplemented.implemented, "NOT_IMPLEMENTED result implemented=false");
    require(!notImplemented.replayExecuted, "NOT_IMPLEMENTED result replayExecuted=false");
    require(notImplemented.status == kReplayStatusNotImplemented, "status is NOT_IMPLEMENTED");
    require(hasReplayIssueCode(notImplemented, "REPLAY_NOT_IMPLEMENTED"), "issues include REPLAY_NOT_IMPLEMENTED");
    require(notImplemented.metadata.sourceFixtureId == "FX004_SELL_EXCEEDS_POSITION", "metadata sourceFixtureId matches");
    require(notImplemented.metadata.fixtureIssueCount == 1, "metadata fixtureIssueCount matches");
    require(notImplemented.metadata.fixtureBlockingExpected, "metadata fixtureBlockingExpected matches");
    require(hasEmptyReplayOutputs(notImplemented), "NOT_IMPLEMENTED result raw outputs are empty");

    const auto invalid = AccountingReplayResultMapper::makeInvalidFixtureResult("Fixture id is empty.");
    require(!invalid.implemented, "invalid result implemented=false");
    require(!invalid.replayExecuted, "invalid result replayExecuted=false");
    require(invalid.status == kReplayStatusInvalidFixture, "invalid result status is INVALID_FIXTURE");
    require(hasReplayIssueCode(invalid, "INVALID_FIXTURE"), "invalid result has INVALID_FIXTURE issue");
    require(hasEmptyReplayOutputs(invalid), "invalid result raw outputs are empty");

    AccountingReplayStubEngine engine;
    const auto stubFx001 = engine.previewFixtureResult(fx001);
    require(stubFx001.status == kReplayStatusNotImplemented, "stub FX001 returns NOT_IMPLEMENTED");
    require(hasReplayIssueCode(stubFx001, "REPLAY_NOT_IMPLEMENTED"), "stub FX001 includes replay issue");
    require(stubFx001.metadata.fixtureIssueCount == 0, "stub FX001 metadata issue count is zero");
    require(!stubFx001.metadata.fixtureBlockingExpected, "stub FX001 blocking expected is false");
    require(hasEmptyReplayOutputs(stubFx001), "stub FX001 raw outputs are empty");

    const auto stubFx004 = engine.previewFixtureResult(fx004);
    require(stubFx004.status == kReplayStatusNotImplemented, "stub FX004 returns NOT_IMPLEMENTED");
    require(stubFx004.metadata.fixtureBlockingExpected, "stub FX004 keeps blocking expectation");
    require(stubFx004.metadata.fixtureIssueCount == 1, "stub FX004 keeps issue count");
    require(hasEmptyReplayOutputs(stubFx004), "stub FX004 raw outputs are empty");

    std::cout << "Accounting replay result skeleton tests passed\n";
    return 0;
}
