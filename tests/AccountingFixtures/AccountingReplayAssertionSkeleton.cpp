#include "AccountingReplayAssertionSkeleton.h"

#include "AccountingExpectedOutputInspector.h"

#include <utility>

namespace etfdt::tests::accounting {
namespace {

constexpr const char* kFutureReplayTask =
    "Enable fixture-backed replay and replace SKIPPED_BY_DESIGN checks with real assertions.";

AccountingAssertionResult makeResult(
    const std::string& fixtureId,
    bool passed,
    std::string status,
    std::string message,
    bool skippedRealAssertions,
    std::vector<std::string> checkedFields)
{
    return AccountingAssertionResult{
        fixtureId,
        true,
        passed,
        std::move(status),
        std::move(message),
        skippedRealAssertions,
        kFutureReplayTask,
        std::move(checkedFields),
    };
}

AccountingAssertionResult makeSkipped(
    const std::string& fixtureId,
    std::string message,
    std::vector<std::string> checkedFields)
{
    return makeResult(
        fixtureId,
        true,
        kAssertionSkippedByDesign,
        std::move(message),
        true,
        std::move(checkedFields));
}

} // namespace

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertExpectedOutputShape(
    const AccountingFixture& fixture) const
{
    AccountingExpectedOutputInspector inspector;
    const auto shape = inspector.inspect(fixture);
    const bool passed = shape.hasPositionSummaries && shape.hasCashSummary && shape.hasPortfolioPnl;
    return makeResult(
        fixture.fixtureId,
        passed,
        passed ? "PASS_EXPECTED_OUTPUT_SHAPE" : kAssertionFailMissingExpectedOutput,
        passed ? "Required expectedOutputs shape is present." : shape.lastError,
        false,
        {"expectedOutputs.positionSummaries", "expectedOutputs.cashSummary", "expectedOutputs.portfolioPnl"});
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertCurrentStubResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    const auto shapeResult = assertExpectedOutputShape(fixture);
    if (!shapeResult.passed) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailMissingExpectedOutput,
            shapeResult.message,
            false,
            shapeResult.checkedFields);
    }

    if (result.implemented || result.replayExecuted || result.status != kReplayStatusNotImplemented) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailUnexpectedImplemented,
            "Current stage expects NOT_IMPLEMENTED with implemented=false and replayExecuted=false.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    if (!hasReplayIssueCode(result, "REPLAY_NOT_IMPLEMENTED")) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailUnexpectedImplemented,
            "NOT_IMPLEMENTED result must include REPLAY_NOT_IMPLEMENTED issue.",
            false,
            {"issues"});
    }

    if (!hasEmptyReplayOutputs(result)) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailResultOutputNotEmpty,
            "NOT_IMPLEMENTED result must not carry position, cash, pnl, base, or sniper outputs.",
            false,
            {
                "positionListResponseRaw",
                "cashSummaryRaw",
                "portfolioPnlRaw",
                "basePositionRaw",
                "sniperPoolRaw",
            });
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassNotImplementedGuard,
        "Fixture expectedOutputs shape is present and current result is the expected NOT_IMPLEMENTED guard.",
        true,
        {
            "expectedOutputs",
            "implemented",
            "replayExecuted",
            "status",
            "issues",
            "rawOutputs",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertPositionList(
    const AccountingFixture& fixture,
    const AccountingReplayResult&) const
{
    return makeSkipped(
        fixture.fixtureId,
        "Position list value assertions are skipped until fixture-backed replay is implemented.",
        {"positionSummaries", "positionListResponseRaw"});
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertCashSummary(
    const AccountingFixture& fixture,
    const AccountingReplayResult&) const
{
    return makeSkipped(
        fixture.fixtureId,
        "Cash summary value assertions are skipped until fixture-backed replay is implemented.",
        {"cashSummary", "cashSummaryRaw"});
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertPortfolioPnl(
    const AccountingFixture& fixture,
    const AccountingReplayResult&) const
{
    return makeSkipped(
        fixture.fixtureId,
        "Portfolio PnL value assertions are skipped until fixture-backed replay is implemented.",
        {"portfolioPnl", "portfolioPnlRaw"});
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertIssuesShape(
    const AccountingFixture& fixture) const
{
    AccountingExpectedOutputInspector inspector;
    const auto codes = inspector.expectedIssueCodes(fixture);
    return makeResult(
        fixture.fixtureId,
        true,
        "PASS_ISSUES_SHAPE",
        "Expected issues array is available through the fixture loader.",
        false,
        codes.empty() ? std::vector<std::string>{"expectedIssues"} : codes);
}

} // namespace etfdt::tests::accounting
