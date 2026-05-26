#include "AccountingReplayAssertionSkeleton.h"
#include "AccountingReplayMinimalEngine.h"
#include "AccountingReplayTestHarness.h"

#include <QJsonArray>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

namespace {

using etfdt::tests::accounting::AccountingFixture;
using etfdt::tests::accounting::AccountingReplayAssertionSkeleton;
using etfdt::tests::accounting::AccountingReplayMinimalEngine;
using etfdt::tests::accounting::AccountingReplayResult;
using etfdt::tests::accounting::AccountingReplayTestHarness;
using etfdt::tests::accounting::hasCashSummary;
using etfdt::tests::accounting::hasPortfolioPnl;
using etfdt::tests::accounting::hasPositionListResponse;
using etfdt::tests::accounting::kAssertionPassFx001EmptyLedger;
using etfdt::tests::accounting::kAssertionPassNotImplementedGuard;
using etfdt::tests::accounting::kReplayStatusInvalidFixture;
using etfdt::tests::accounting::kReplayStatusNotImplemented;
using etfdt::tests::accounting::kReplayStatusOk;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(1);
    }
}

template <typename T>
T requireValue(const std::optional<T>& value, const std::string& message)
{
    require(value.has_value(), message);
    return *value;
}

std::filesystem::path fixturesDirFromArgs(int argc, char** argv)
{
    if (argc >= 2) {
        return std::filesystem::path(argv[1]);
    }
    return std::filesystem::path("tests") / "fixtures" / "accounting_replay";
}

AccountingReplayResult requireResult(const AccountingReplayTestHarness& harness, const std::string& fixtureId)
{
    return requireValue(harness.resultForFixture(fixtureId), "result exists for " + fixtureId);
}

bool hasBlockingIssue(const AccountingReplayResult& result)
{
    for (const auto& issue : result.issues) {
        if (issue.blocking) {
            return true;
        }
    }
    return false;
}

} // namespace

int main(int argc, char** argv)
{
    const auto fixturesDir = fixturesDirFromArgs(argc, argv);

    AccountingReplayTestHarness harness;
    require(harness.loadFixtures(fixturesDir), "harness loads fixtures");

    const auto fx001 = requireValue(harness.fixtureByIdForTest("FX001_EMPTY_LEDGER"), "FX001 fixture exists");
    const auto fx002 = requireValue(harness.fixtureByIdForTest("FX002_SINGLE_BUY"), "FX002 fixture exists");
    const auto fx004 = requireValue(harness.fixtureByIdForTest("FX004_SELL_EXCEEDS_POSITION"), "FX004 fixture exists");

    AccountingReplayMinimalEngine engine;
    require(engine.supportsFixture("FX001_EMPTY_LEDGER"), "minimal engine supports FX001");
    require(!engine.supportsFixture("FX002_SINGLE_BUY"), "minimal engine does not support FX002");

    const auto fx001Direct = engine.replayFixture(fx001);
    require(fx001Direct.implemented, "FX001 direct replay implemented=true");
    require(fx001Direct.replayExecuted, "FX001 direct replayExecuted=true");
    require(fx001Direct.status == kReplayStatusOk, "FX001 direct status=OK");
    require(hasPositionListResponse(fx001Direct), "FX001 has positionListResponseRaw");
    require(fx001Direct.positionListResponseRaw.value("positions").isArray(), "FX001 positions is an array");
    require(fx001Direct.positionListResponseRaw.value("positions").toArray().isEmpty(), "FX001 positions array is empty");
    require(hasCashSummary(fx001Direct), "FX001 has cashSummaryRaw");
    require(hasPortfolioPnl(fx001Direct), "FX001 has portfolioPnlRaw");
    require(!hasBlockingIssue(fx001Direct), "FX001 has no blocking issue");

    const auto fx002Direct = engine.replayFixture(fx002);
    require(!fx002Direct.implemented, "FX002 direct replay remains implemented=false");
    require(!fx002Direct.replayExecuted, "FX002 direct replayExecuted=false");
    require(fx002Direct.status == kReplayStatusNotImplemented, "FX002 direct status=NOT_IMPLEMENTED");

    const auto fx004Direct = engine.replayFixture(fx004);
    require(fx004Direct.status == kReplayStatusNotImplemented, "FX004 direct status=NOT_IMPLEMENTED");
    require(fx004Direct.metadata.fixtureBlockingExpected, "FX004 keeps blocking expected metadata");

    AccountingFixture emptyFixture;
    const auto invalid = engine.replayFixture(emptyFixture);
    require(invalid.status == kReplayStatusInvalidFixture, "empty fixture id returns INVALID_FIXTURE");

    require(harness.runAllWithMinimalEngine(), "harness runs all fixtures with minimal engine");
    require(harness.results().size() == 13, "minimal harness result count is 13");

    const auto fx001Harness = requireResult(harness, "FX001_EMPTY_LEDGER");
    require(fx001Harness.status == kReplayStatusOk, "minimal harness FX001 status=OK");
    require(fx001Harness.implemented, "minimal harness FX001 implemented=true");
    require(fx001Harness.replayExecuted, "minimal harness FX001 replayExecuted=true");

    AccountingReplayAssertionSkeleton assertions;
    const auto fx001Assertion = assertions.assertFx001EmptyLedgerResult(fx001, fx001Harness);
    require(fx001Assertion.passed, "FX001 empty-ledger assertion passes");
    require(fx001Assertion.status == kAssertionPassFx001EmptyLedger, "FX001 assertion status is PASS_FX001_EMPTY_LEDGER");

    for (const auto& fixtureId : harness.coveredFixtureIds()) {
        const auto fixture = requireValue(harness.fixtureByIdForTest(fixtureId), "fixture exists for minimal result loop");
        const auto result = requireResult(harness, fixtureId);
        if (fixtureId == "FX001_EMPTY_LEDGER") {
            require(result.status == kReplayStatusOk, "FX001 remains OK in minimal loop");
            continue;
        }

        require(result.status == kReplayStatusNotImplemented, fixtureId + " remains NOT_IMPLEMENTED");
        require(!result.implemented, fixtureId + " remains implemented=false");
        require(!result.replayExecuted, fixtureId + " remains replayExecuted=false");
        const auto assertion = assertions.assertCurrentStubResult(fixture, result);
        require(assertion.passed, fixtureId + " still passes NOT_IMPLEMENTED guard assertion");
        require(assertion.status == kAssertionPassNotImplementedGuard, fixtureId + " guard status is PASS_NOT_IMPLEMENTED_GUARD");
    }

    std::cout << "Accounting replay minimal FX001 tests passed\n";
    return 0;
}
