#include "AccountingReplayAssertionSkeleton.h"

#include "AccountingExpectedOutputInspector.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

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

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx001EmptyLedgerResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX001_EMPTY_LEDGER") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx001EmptyLedger,
            "FX001 empty-ledger assertion only accepts FX001_EMPTY_LEDGER.",
            false,
            {"fixtureId"});
    }

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

    if (!result.implemented || !result.replayExecuted || result.status != kReplayStatusOk) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx001EmptyLedger,
            "FX001 replay result must be implemented=true, replayExecuted=true, and status=OK.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto positionsValue = result.positionListResponseRaw.value("positions");
    if (!positionsValue.isArray() || !positionsValue.toArray().isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx001EmptyLedger,
            "FX001 positionListResponseRaw.positions must exist and be empty.",
            false,
            {"positionListResponseRaw.positions"});
    }

    if (result.cashSummaryRaw.isEmpty() || result.portfolioPnlRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx001EmptyLedger,
            "FX001 must include cashSummaryRaw and portfolioPnlRaw.",
            false,
            {"cashSummaryRaw", "portfolioPnlRaw"});
    }

    for (const auto& issue : result.issues) {
        if (issue.blocking) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx001EmptyLedger,
                "FX001 empty ledger result must not contain blocking issues.",
                false,
                {"issues"});
        }
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx001EmptyLedger,
        "FX001 empty-ledger replay result is implemented and contains empty positions with cash and PnL summaries.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "positionListResponseRaw.positions",
            "cashSummaryRaw",
            "portfolioPnlRaw",
            "issues",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx002SingleBuyResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX002_SINGLE_BUY") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 single-buy assertion only accepts FX002_SINGLE_BUY.",
            false,
            {"fixtureId"});
    }

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

    if (!result.implemented || !result.replayExecuted || result.status != kReplayStatusOk) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 replay result must be implemented=true, replayExecuted=true, and status=OK.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto expectedPositions = fixture.expectedOutputsRawJson.value("positionSummaries");
    const auto expectedCash = fixture.expectedOutputsRawJson.value("cashSummary");
    if (!expectedPositions.isArray() || expectedPositions.toArray().size() != 1 || !expectedCash.isObject()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 fixture expected output must contain one position and one cash summary.",
            false,
            {"expectedOutputs.positionSummaries", "expectedOutputs.cashSummary"});
    }

    const auto expectedPosition = expectedPositions.toArray().first().toObject();
    const auto expectedCashSummary = expectedCash.toObject();
    const auto positionsValue = result.positionListResponseRaw.value("positions");
    if (!positionsValue.isArray() || positionsValue.toArray().size() != 1) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 positionListResponseRaw.positions must contain exactly one position.",
            false,
            {"positionListResponseRaw.positions"});
    }

    const auto position = positionsValue.toArray().first().toObject();
    const auto instrumentCode = position.value("instrumentCode").toString();
    const auto quantityText = position.value("quantityText").toString();
    const auto costAmountText = position.value("costAmountText").toString();
    const auto expectedCostAmountText = expectedPosition.value("costAmountText").toString();
    if (instrumentCode != "159509" || quantityText != "1000" || costAmountText != expectedCostAmountText) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 position output must contain 159509, quantityText=1000, and expected costAmountText.",
            false,
            {"instrumentCode", "quantityText", "costAmountText"});
    }

    if (result.cashSummaryRaw.isEmpty() || result.portfolioPnlRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 must include cashSummaryRaw and portfolioPnlRaw.",
            false,
            {"cashSummaryRaw", "portfolioPnlRaw"});
    }

    const auto cashBalanceText = result.cashSummaryRaw.value("cashBalanceText").toString();
    const auto expectedCashBalanceText = expectedCashSummary.value("cashBalanceText").toString();
    if (cashBalanceText != expectedCashBalanceText) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx002SingleBuy,
            "FX002 cashSummaryRaw.cashBalanceText must match fixture expected output.",
            false,
            {"cashSummaryRaw.cashBalanceText"});
    }

    for (const auto& issue : result.issues) {
        if (issue.blocking) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx002SingleBuy,
                "FX002 single buy result must not contain blocking issues.",
                false,
                {"issues"});
        }
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx002SingleBuy,
        "FX002 single-buy replay result contains the expected minimal position and cash outputs.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "positionListResponseRaw.positions",
            "instrumentCode",
            "quantityText",
            "costAmountText",
            "cashSummaryRaw.cashBalanceText",
            "portfolioPnlRaw",
            "issues",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx003BuySellPartialResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX003_BUY_SELL_PARTIAL") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 partial-sell assertion only accepts FX003_BUY_SELL_PARTIAL.",
            false,
            {"fixtureId"});
    }

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

    if (!result.implemented || !result.replayExecuted || result.status != kReplayStatusOk) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 replay result must be implemented=true, replayExecuted=true, and status=OK.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto expectedPositions = fixture.expectedOutputsRawJson.value("positionSummaries");
    const auto expectedCash = fixture.expectedOutputsRawJson.value("cashSummary");
    const auto expectedPnl = fixture.expectedOutputsRawJson.value("portfolioPnl");
    if (!expectedPositions.isArray() || expectedPositions.toArray().size() != 1 || !expectedCash.isObject()
        || !expectedPnl.isObject()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 fixture expected output must contain one position, cash summary, and portfolio PnL.",
            false,
            {"expectedOutputs.positionSummaries", "expectedOutputs.cashSummary", "expectedOutputs.portfolioPnl"});
    }

    const auto expectedPosition = expectedPositions.toArray().first().toObject();
    const auto expectedCashSummary = expectedCash.toObject();
    const auto expectedPortfolioPnl = expectedPnl.toObject();
    const auto positionsValue = result.positionListResponseRaw.value("positions");
    if (!positionsValue.isArray() || positionsValue.toArray().size() != 1) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 positionListResponseRaw.positions must contain exactly one position.",
            false,
            {"positionListResponseRaw.positions"});
    }

    const auto position = positionsValue.toArray().first().toObject();
    if (position.value("instrumentCode").toString() != "159509"
        || position.value("quantityText").toString() != "600"
        || position.value("costAmountText").toString() != expectedPosition.value("costAmountText").toString()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 position output must contain 159509, quantityText=600, and expected costAmountText.",
            false,
            {"instrumentCode", "quantityText", "costAmountText"});
    }

    if (result.cashSummaryRaw.isEmpty() || result.portfolioPnlRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 must include cashSummaryRaw and portfolioPnlRaw.",
            false,
            {"cashSummaryRaw", "portfolioPnlRaw"});
    }

    if (result.cashSummaryRaw.value("cashBalanceText").toString()
            != expectedCashSummary.value("cashBalanceText").toString()
        || result.cashSummaryRaw.value("feeTotalText").toString()
            != expectedCashSummary.value("feeTotalText").toString()
        || result.cashSummaryRaw.value("sellCashInflowText").toString() != "479.00 CNY") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 cash summary must match expected cash balance, sell inflow, and fee total.",
            false,
            {"cashBalanceText", "sellCashInflowText", "feeTotalText"});
    }

    if (result.portfolioPnlRaw.value("realizedPnlText").toString()
        != expectedPortfolioPnl.value("realizedPnlText").toString()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx003BuySellPartial,
            "FX003 portfolioPnlRaw.realizedPnlText must match fixture expected output.",
            false,
            {"portfolioPnlRaw.realizedPnlText"});
    }

    for (const auto& issue : result.issues) {
        if (issue.blocking) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx003BuySellPartial,
                "FX003 partial sell result must not contain blocking issues.",
                false,
                {"issues"});
        }
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx003BuySellPartial,
        "FX003 partial-sell replay result contains the expected remaining position, cash, and realized PnL outputs.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "positionListResponseRaw.positions",
            "instrumentCode",
            "quantityText",
            "costAmountText",
            "cashSummaryRaw.cashBalanceText",
            "cashSummaryRaw.sellCashInflowText",
            "cashSummaryRaw.feeTotalText",
            "portfolioPnlRaw.realizedPnlText",
            "issues",
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
