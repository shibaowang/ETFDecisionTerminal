#include "AccountingReplayStubEngine.h"
#include "AccountingReplayTestHarness.h"
#include "AccountingReplayResult.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

using etfdt::tests::accounting::AccountingFixture;
using etfdt::tests::accounting::AccountingReplayStubEngine;
using etfdt::tests::accounting::AccountingReplayTestHarness;
using etfdt::tests::accounting::hasEmptyReplayOutputs;
using etfdt::tests::accounting::hasReplayIssueCode;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(1);
    }
}

std::filesystem::path fixtureDirFromArgs(int argc, char** argv)
{
    if (argc >= 2) {
        return std::filesystem::path(argv[1]);
    }
    return std::filesystem::path("tests") / "fixtures" / "accounting_replay";
}

std::vector<std::string> expectedFixtureIds()
{
    return {
        "FX001_EMPTY_LEDGER",
        "FX002_SINGLE_BUY",
        "FX003_BUY_SELL_PARTIAL",
        "FX004_SELL_EXCEEDS_POSITION",
        "FX005_MISSING_FEE",
        "FX006_NEGATIVE_CASH",
        "FX007_MULTI_INSTRUMENT",
        "FX008_MULTI_ACCOUNT",
        "FX009_BASE_POSITION_LOCKED",
        "FX010_SNIPER_TIER_COMPLETED",
        "FX011_STALE_SNAPSHOT",
        "FX012_MISSING_MARKET_PRICE",
        "FX013_MULTI_CURRENCY_UNSUPPORTED",
    };
}

bool containsId(const std::vector<std::string>& ids, const std::string& id)
{
    for (const auto& value : ids) {
        if (value == id) {
            return true;
        }
    }
    return false;
}

} // namespace

int main(int argc, char** argv)
{
    const auto fixturesDir = fixtureDirFromArgs(argc, argv);
    const auto expectedIds = expectedFixtureIds();

    AccountingReplayTestHarness harness;
    require(harness.loadFixtures(fixturesDir), "harness loadFixtures succeeds: " + harness.lastError());
    require(harness.runAll(), "harness runAll succeeds: " + harness.lastError());
    require(harness.results().size() == 13, "harness returns 13 results");

    const auto covered = harness.coveredFixtureIds();
    require(containsId(covered, "FX001_EMPTY_LEDGER"), "covered fixtures include FX001");
    require(containsId(covered, "FX013_MULTI_CURRENCY_UNSUPPORTED"), "covered fixtures include FX013");
    require(harness.missingFixtureIds(expectedIds).empty(), "no expected fixtures are missing");

    for (const auto& result : harness.results()) {
        require(!result.implemented, result.fixtureId + " implemented is false");
        require(!result.replayExecuted, result.fixtureId + " replayExecuted is false");
        require(result.status == "NOT_IMPLEMENTED", result.fixtureId + " status is NOT_IMPLEMENTED");
        require(result.metadata.sourceFixtureId == result.fixtureId, result.fixtureId + " sourceFixtureId matches");
        require(result.metadata.requiredFutureTask == "Implement fixture-backed accounting replay.", result.fixtureId + " future task is explicit");
        require(hasReplayIssueCode(result, "REPLAY_NOT_IMPLEMENTED"), result.fixtureId + " issue includes REPLAY_NOT_IMPLEMENTED");
        require(hasEmptyReplayOutputs(result), result.fixtureId + " raw replay outputs are empty");
    }

    const auto fx004 = harness.resultForFixture("FX004_SELL_EXCEEDS_POSITION");
    require(fx004.has_value(), "FX004 result exists");
    require(fx004->metadata.fixtureBlockingExpected, "FX004 blockingExpected is true");
    require(fx004->metadata.fixtureIssueCount == 1, "FX004 fixtureIssueCount is one");

    const auto fx001 = harness.resultForFixture("FX001_EMPTY_LEDGER");
    require(fx001.has_value(), "FX001 result exists");
    require(!fx001->metadata.fixtureBlockingExpected, "FX001 blockingExpected is false");
    require(fx001->metadata.fixtureIssueCount == 0, "FX001 issueCount is zero");

    require(!harness.resultForFixture("FX999_MISSING").has_value(), "missing result returns empty optional");

    AccountingReplayStubEngine engine;
    const AccountingFixture emptyFixture{};
    const auto invalidResult = engine.previewFixture(emptyFixture);
    require(!invalidResult.implemented, "invalid fixture result implemented is false");
    require(!invalidResult.replayExecuted, "invalid fixture result replayExecuted is false");
    require(invalidResult.status == "INVALID_FIXTURE", "empty fixture id returns INVALID_FIXTURE");

    AccountingReplayTestHarness invalidHarness;
    require(!invalidHarness.loadFixtures(fixturesDir / "missing"), "invalid fixture dir fails");
    require(!invalidHarness.lastError().empty(), "invalid fixture dir returns an error");

    std::cout << "Accounting replay harness skeleton tests passed\n";
    return 0;
}
