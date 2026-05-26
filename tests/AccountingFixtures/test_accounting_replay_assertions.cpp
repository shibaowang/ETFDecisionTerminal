#include "AccountingExpectedOutputInspector.h"
#include "AccountingReplayAssertionSkeleton.h"
#include "AccountingReplayTestHarness.h"

#include <QJsonObject>
#include <QString>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <set>
#include <string>

namespace {

using etfdt::tests::accounting::AccountingExpectedOutputInspector;
using etfdt::tests::accounting::AccountingReplayAssertionSkeleton;
using etfdt::tests::accounting::AccountingReplayResult;
using etfdt::tests::accounting::AccountingReplayTestHarness;
using etfdt::tests::accounting::hasEmptyReplayOutputs;
using etfdt::tests::accounting::kAssertionFailResultOutputNotEmpty;
using etfdt::tests::accounting::kAssertionFailUnexpectedImplemented;
using etfdt::tests::accounting::kAssertionPassNotImplementedGuard;
using etfdt::tests::accounting::kAssertionSkippedByDesign;
using etfdt::tests::accounting::kReplayStatusNotImplemented;

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

} // namespace

int main(int argc, char** argv)
{
    const auto fixturesDir = fixturesDirFromArgs(argc, argv);

    AccountingReplayTestHarness harness;
    require(harness.loadFixtures(fixturesDir), "harness loads fixtures");
    require(harness.runAll(), "harness runs all fixtures");
    require(harness.results().size() == 13, "harness returns 13 results");

    AccountingExpectedOutputInspector inspector;
    AccountingReplayAssertionSkeleton assertions;

    const auto fx001 = requireValue(harness.fixtureByIdForTest("FX001_EMPTY_LEDGER"), "FX001 fixture exists");
    const auto fx002 = requireValue(harness.fixtureByIdForTest("FX002_SINGLE_BUY"), "FX002 fixture exists");
    const auto fx004 = requireValue(harness.fixtureByIdForTest("FX004_SELL_EXCEEDS_POSITION"), "FX004 fixture exists");
    const auto fx005 = requireValue(harness.fixtureByIdForTest("FX005_MISSING_FEE"), "FX005 fixture exists");
    const auto fx006 = requireValue(harness.fixtureByIdForTest("FX006_NEGATIVE_CASH"), "FX006 fixture exists");
    const auto fx011 = requireValue(harness.fixtureByIdForTest("FX011_STALE_SNAPSHOT"), "FX011 fixture exists");
    const auto fx012 = requireValue(harness.fixtureByIdForTest("FX012_MISSING_MARKET_PRICE"), "FX012 fixture exists");
    const auto fx013 = requireValue(harness.fixtureByIdForTest("FX013_MULTI_CURRENCY_UNSUPPORTED"), "FX013 fixture exists");

    const auto shapeFx001 = inspector.inspect(fx001);
    require(shapeFx001.hasPositionSummaries, "FX001 has positionSummaries field");
    require(shapeFx001.positionSummaryCount == 0, "FX001 positionSummaries is empty");

    const auto shapeFx002 = inspector.inspect(fx002);
    require(shapeFx002.hasPositionSummaries, "FX002 has positionSummaries");
    require(shapeFx002.positionSummaryCount >= 1, "FX002 has at least one position summary");
    require(shapeFx002.hasCashSummary, "FX002 has cashSummary");
    require(shapeFx002.hasPortfolioPnl, "FX002 has portfolioPnl");

    require(inspector.hasErrorCode(fx004, "SELL_EXCEEDS_POSITION"), "FX004 expectedIssues includes SELL_EXCEEDS_POSITION");
    require(inspector.hasErrorCode(fx005, "MISSING_FEE"), "FX005 expectedIssues includes MISSING_FEE");
    require(inspector.hasErrorCode(fx006, "NEGATIVE_CASH"), "FX006 expectedIssues includes NEGATIVE_CASH");
    require(inspector.hasErrorCode(fx011, "SNAPSHOT_STALE"), "FX011 expectedIssues includes SNAPSHOT_STALE");
    require(inspector.hasErrorCode(fx012, "MARKET_PRICE_MISSING"), "FX012 expectedIssues includes MARKET_PRICE_MISSING");
    require(
        inspector.hasErrorCode(fx013, "MULTI_CURRENCY_UNSUPPORTED") || inspector.hasErrorCode(fx013, "FX_RATE_MISSING"),
        "FX013 expectedIssues includes multi-currency or FX issue");

    const auto fx001Result = requireResult(harness, "FX001_EMPTY_LEDGER");
    const auto fx001Assertion = assertions.assertCurrentStubResult(fx001, fx001Result);
    require(fx001Assertion.passed, "FX001 NOT_IMPLEMENTED guard assertion passes");
    require(fx001Assertion.status == kAssertionPassNotImplementedGuard, "FX001 assertion has PASS_NOT_IMPLEMENTED_GUARD status");
    require(fx001Assertion.skippedRealAssertions, "FX001 real assertions are skipped by design");

    const auto fx004Result = requireResult(harness, "FX004_SELL_EXCEEDS_POSITION");
    const auto fx004Assertion = assertions.assertCurrentStubResult(fx004, fx004Result);
    require(fx004Assertion.passed, "FX004 NOT_IMPLEMENTED guard assertion passes");
    require(fx004Assertion.status == kAssertionPassNotImplementedGuard, "FX004 assertion has PASS_NOT_IMPLEMENTED_GUARD status");

    auto implementedResult = fx001Result;
    implementedResult.implemented = true;
    const auto implementedAssertion = assertions.assertCurrentStubResult(fx001, implementedResult);
    require(!implementedAssertion.passed, "implemented=true fake result fails");
    require(implementedAssertion.status == kAssertionFailUnexpectedImplemented, "implemented=true failure status is explicit");

    auto nonEmptyResult = fx001Result;
    nonEmptyResult.positionListResponseRaw.insert(QStringLiteral("unexpected"), QStringLiteral("value"));
    require(!hasEmptyReplayOutputs(nonEmptyResult), "test fixture creates non-empty raw output");
    const auto nonEmptyAssertion = assertions.assertCurrentStubResult(fx001, nonEmptyResult);
    require(!nonEmptyAssertion.passed, "NOT_IMPLEMENTED result with raw output fails");
    require(nonEmptyAssertion.status == kAssertionFailResultOutputNotEmpty, "raw output failure status is explicit");

    const auto skippedPosition = assertions.assertPositionList(fx002, requireResult(harness, "FX002_SINGLE_BUY"));
    require(skippedPosition.status == kAssertionSkippedByDesign, "position assertion is SKIPPED_BY_DESIGN");
    const auto skippedCash = assertions.assertCashSummary(fx002, requireResult(harness, "FX002_SINGLE_BUY"));
    require(skippedCash.status == kAssertionSkippedByDesign, "cash assertion is SKIPPED_BY_DESIGN");
    const auto skippedPnl = assertions.assertPortfolioPnl(fx002, requireResult(harness, "FX002_SINGLE_BUY"));
    require(skippedPnl.status == kAssertionSkippedByDesign, "portfolio pnl assertion is SKIPPED_BY_DESIGN");

    std::set<std::string> assertedFixtureIds;
    for (const auto& fixtureId : harness.coveredFixtureIds()) {
        const auto fixture = requireValue(harness.fixtureByIdForTest(fixtureId), "fixture exists for assertion loop");
        const auto result = requireResult(harness, fixtureId);
        const auto assertion = assertions.assertCurrentStubResult(fixture, result);
        require(assertion.passed, "assertCurrentStubResult passes for " + fixtureId);
        require(result.status == kReplayStatusNotImplemented, "result remains NOT_IMPLEMENTED for " + fixtureId);
        assertedFixtureIds.insert(fixtureId);
    }
    require(assertedFixtureIds.size() == 13, "all 13 fixtures enter assertion skeleton");
    require(assertedFixtureIds.count("FX004_SELL_EXCEEDS_POSITION") == 1, "error fixture FX004 is asserted");
    require(assertedFixtureIds.count("FX013_MULTI_CURRENCY_UNSUPPORTED") == 1, "error fixture FX013 is asserted");

    std::cout << "Accounting replay expected-output assertion skeleton tests passed\n";
    return 0;
}
