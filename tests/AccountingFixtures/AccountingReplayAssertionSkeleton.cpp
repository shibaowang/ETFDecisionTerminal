#include "AccountingReplayAssertionSkeleton.h"

#include "AccountingExpectedOutputInspector.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <algorithm>
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

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx004SellExceedsPositionResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX004_SELL_EXCEEDS_POSITION") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx004SellExceedsPosition,
            "FX004 sell-exceeds-position assertion only accepts FX004_SELL_EXCEEDS_POSITION.",
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

    if (!result.implemented || !result.replayExecuted || result.status != kReplayStatusError) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx004SellExceedsPosition,
            "FX004 replay result must be implemented=true, replayExecuted=true, and status=ERROR.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    bool hasSellExceeds = false;
    bool sellExceedsBlocking = false;
    for (const auto& issue : result.issues) {
        if (issue.code == "SELL_EXCEEDS_POSITION") {
            hasSellExceeds = true;
            sellExceedsBlocking = issue.blocking;
        }
    }
    if (!hasSellExceeds || !sellExceedsBlocking) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx004SellExceedsPosition,
            "FX004 must contain a blocking SELL_EXCEEDS_POSITION issue.",
            false,
            {"issues.SELL_EXCEEDS_POSITION"});
    }

    if (!hasEmptyReplayOutputs(result)) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx004SellExceedsPosition,
            "FX004 must not generate normal position, cash, PnL, base-position, or sniper-pool outputs.",
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
        kAssertionPassFx004SellExceedsPosition,
        "FX004 oversell replay result exposes a blocking SELL_EXCEEDS_POSITION error without normal outputs.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "issues.SELL_EXCEEDS_POSITION",
            "positionListResponseRaw",
            "cashSummaryRaw",
            "portfolioPnlRaw",
            "basePositionRaw",
            "sniperPoolRaw",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx005MissingFeeResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX005_MISSING_FEE") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx005MissingFee,
            "FX005 missing-fee assertion only accepts FX005_MISSING_FEE.",
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

    const auto expectedIssue = std::find_if(
        fixture.expectedIssues.begin(),
        fixture.expectedIssues.end(),
        [](const AccountingExpectedIssue& issue) { return issue.code == "MISSING_FEE"; });
    if (expectedIssue == fixture.expectedIssues.end()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx005MissingFee,
            "FX005 fixture must declare an expected MISSING_FEE issue.",
            false,
            {"expectedIssues.MISSING_FEE"});
    }

    const std::string expectedStatus = expectedIssue->level == "ERROR" ? kReplayStatusError : kReplayStatusWarning;
    if (!result.implemented || !result.replayExecuted || result.status != expectedStatus) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx005MissingFee,
            "FX005 replay result must be implemented=true, replayExecuted=true, and match the fixture status.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    bool hasMissingFee = false;
    bool blockingMatches = false;
    for (const auto& issue : result.issues) {
        if (issue.code == "MISSING_FEE") {
            hasMissingFee = true;
            blockingMatches = issue.blocking == expectedIssue->blocking;
        }
    }
    if (!hasMissingFee || !blockingMatches) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx005MissingFee,
            "FX005 must contain MISSING_FEE with blocking matching the fixture expected issue.",
            false,
            {"issues.MISSING_FEE"});
    }

    if (!hasEmptyReplayOutputs(result)) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx005MissingFee,
            "FX005 must not generate normal position, cash, PnL, base-position, or sniper-pool outputs.",
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
        kAssertionPassFx005MissingFee,
        "FX005 missing-fee replay result exposes MISSING_FEE without normal outputs or fee=0 fallback.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "issues.MISSING_FEE",
            "positionListResponseRaw",
            "cashSummaryRaw",
            "portfolioPnlRaw",
            "basePositionRaw",
            "sniperPoolRaw",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx006NegativeCashResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX006_NEGATIVE_CASH") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx006NegativeCash,
            "FX006 negative-cash assertion only accepts FX006_NEGATIVE_CASH.",
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

    const auto expectedIssue = std::find_if(
        fixture.expectedIssues.begin(),
        fixture.expectedIssues.end(),
        [](const AccountingExpectedIssue& issue) { return issue.code == "NEGATIVE_CASH"; });
    if (expectedIssue == fixture.expectedIssues.end()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx006NegativeCash,
            "FX006 fixture must declare an expected NEGATIVE_CASH issue.",
            false,
            {"expectedIssues.NEGATIVE_CASH"});
    }

    if (!result.implemented || !result.replayExecuted || result.status != kReplayStatusError) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx006NegativeCash,
            "FX006 replay result must be implemented=true, replayExecuted=true, and status=ERROR.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    bool hasNegativeCash = false;
    bool negativeCashBlocking = false;
    for (const auto& issue : result.issues) {
        if (issue.code == "NEGATIVE_CASH") {
            hasNegativeCash = true;
            negativeCashBlocking = issue.blocking;
        }
    }
    if (!hasNegativeCash || !negativeCashBlocking || !expectedIssue->blocking) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx006NegativeCash,
            "FX006 must contain a blocking NEGATIVE_CASH issue matching the fixture contract.",
            false,
            {"issues.NEGATIVE_CASH"});
    }

    if (!hasEmptyReplayOutputs(result)) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx006NegativeCash,
            "FX006 must not generate normal position, cash, PnL, base-position, or sniper-pool outputs.",
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
        kAssertionPassFx006NegativeCash,
        "FX006 negative-cash replay result exposes a blocking NEGATIVE_CASH error without normal outputs.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "issues.NEGATIVE_CASH",
            "positionListResponseRaw",
            "cashSummaryRaw",
            "portfolioPnlRaw",
            "basePositionRaw",
            "sniperPoolRaw",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx007MultiInstrumentResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX007_MULTI_INSTRUMENT") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx007MultiInstrument,
            "FX007 multi-instrument assertion only accepts FX007_MULTI_INSTRUMENT.",
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

    const auto expectedIssue = std::find_if(
        fixture.expectedIssues.begin(),
        fixture.expectedIssues.end(),
        [](const AccountingExpectedIssue& issue) { return issue.code == "MARKET_PRICE_MISSING"; });
    const bool expectsMissingPrice = expectedIssue != fixture.expectedIssues.end();
    const std::string expectedStatus = expectsMissingPrice ? kReplayStatusWarning : kReplayStatusOk;
    if (!result.implemented || !result.replayExecuted || result.status != expectedStatus) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx007MultiInstrument,
            "FX007 replay result must be implemented=true, replayExecuted=true, and match expected status.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto expectedPositions = fixture.expectedOutputsRawJson.value("positionSummaries");
    const auto expectedCash = fixture.expectedOutputsRawJson.value("cashSummary");
    if (!expectedPositions.isArray() || expectedPositions.toArray().size() < 2 || !expectedCash.isObject()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx007MultiInstrument,
            "FX007 fixture expected output must contain at least two positions and one cash summary.",
            false,
            {"expectedOutputs.positionSummaries", "expectedOutputs.cashSummary"});
    }

    const auto positionsValue = result.positionListResponseRaw.value("positions");
    if (!positionsValue.isArray() || positionsValue.toArray().size() < 2) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx007MultiInstrument,
            "FX007 positionListResponseRaw.positions must contain at least two positions.",
            false,
            {"positionListResponseRaw.positions"});
    }

    const auto actualPositions = positionsValue.toArray();
    for (const auto& expectedValue : expectedPositions.toArray()) {
        const auto expectedPosition = expectedValue.toObject();
        const auto expectedInstrument = expectedPosition.value("instrumentCode").toString();
        const auto expectedQuantity = expectedPosition.value("quantityText").toString();
        const auto expectedCostAmount = expectedPosition.value("costAmountText").toString();
        bool found = false;
        for (const auto& actualValue : actualPositions) {
            const auto actualPosition = actualValue.toObject();
            if (actualPosition.value("instrumentCode").toString() != expectedInstrument) {
                continue;
            }
            found = true;
            if (actualPosition.value("quantityText").toString() != expectedQuantity) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx007MultiInstrument,
                    "FX007 quantityText must match each expected instrument position.",
                    false,
                    {"positionListResponseRaw.positions.quantityText"});
            }
            if (!expectedCostAmount.isEmpty()
                && actualPosition.value("costAmountText").toString() != expectedCostAmount) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx007MultiInstrument,
                    "FX007 costAmountText must match expected output when specified.",
                    false,
                    {"positionListResponseRaw.positions.costAmountText"});
            }
            if (actualPosition.value("costAmountText").toString().isEmpty()) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx007MultiInstrument,
                    "FX007 must provide costAmountText per instrument.",
                    false,
                    {"positionListResponseRaw.positions.costAmountText"});
            }
            const auto marketValue = actualPosition.value("marketValueText").toString();
            const auto unrealizedPnl = actualPosition.value("unrealizedPnlText").toString();
            if ((!marketValue.isEmpty() && marketValue != "unavailable")
                || (!unrealizedPnl.isEmpty() && unrealizedPnl != "unavailable")) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx007MultiInstrument,
                    "FX007 must not fabricate market value or unrealized PnL when market prices are missing.",
                    false,
                    {"marketValueText", "unrealizedPnlText"});
            }
        }
        if (!found) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx007MultiInstrument,
                "FX007 positions must contain every expected instrumentCode.",
                false,
                {"positionListResponseRaw.positions.instrumentCode"});
        }
    }

    const auto expectedCashSummary = expectedCash.toObject();
    if (result.cashSummaryRaw.isEmpty() || result.cashSummaryRaw.value("cashBalanceText").toString()
            != expectedCashSummary.value("cashBalanceText").toString()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx007MultiInstrument,
            "FX007 cashSummaryRaw.cashBalanceText must match fixture expected output.",
            false,
            {"cashSummaryRaw.cashBalanceText"});
    }

    if (result.portfolioPnlRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx007MultiInstrument,
            "FX007 must include portfolioPnlRaw with unavailable valuation fields.",
            false,
            {"portfolioPnlRaw"});
    }

    if (expectsMissingPrice) {
        bool hasMarketPriceMissing = false;
        bool blockingMatches = false;
        for (const auto& issue : result.issues) {
            if (issue.code == "MARKET_PRICE_MISSING") {
                hasMarketPriceMissing = true;
                blockingMatches = issue.blocking == expectedIssue->blocking;
            }
        }
        if (!hasMarketPriceMissing || !blockingMatches) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx007MultiInstrument,
                "FX007 must contain MARKET_PRICE_MISSING with blocking matching the fixture expected issue.",
                false,
                {"issues.MARKET_PRICE_MISSING"});
        }
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx007MultiInstrument,
        "FX007 multi-instrument replay result keeps positions separated and reports missing market prices.",
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
            "issues.MARKET_PRICE_MISSING",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx008MultiAccountResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX008_MULTI_ACCOUNT") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 multi-account assertion only accepts FX008_MULTI_ACCOUNT.",
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

    const auto expectedIssue = std::find_if(
        fixture.expectedIssues.begin(),
        fixture.expectedIssues.end(),
        [](const AccountingExpectedIssue& issue) { return issue.code == "MARKET_PRICE_MISSING"; });
    const bool expectsMissingPrice = expectedIssue != fixture.expectedIssues.end();
    const std::string expectedStatus = expectsMissingPrice ? kReplayStatusWarning : kReplayStatusOk;
    if (!result.implemented || !result.replayExecuted || result.status != expectedStatus) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 replay result must be implemented=true, replayExecuted=true, and match expected status.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto expectedPositions = fixture.expectedOutputsRawJson.value("positionSummaries");
    if (!expectedPositions.isArray() || expectedPositions.toArray().size() < 2) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 fixture expected output must contain at least two account-scoped positions.",
            false,
            {"expectedOutputs.positionSummaries"});
    }

    const auto positionsValue = result.positionListResponseRaw.value("positions");
    if (!positionsValue.isArray() || positionsValue.toArray().size() < 2) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 positionListResponseRaw.positions must contain at least two positions.",
            false,
            {"positionListResponseRaw.positions"});
    }

    const auto actualPositions = positionsValue.toArray();
    bool sawAccountOne = false;
    bool sawAccountTwo = false;
    for (const auto& expectedValue : expectedPositions.toArray()) {
        const auto expectedPosition = expectedValue.toObject();
        const auto expectedAccount = expectedPosition.value("accountId").toString();
        const auto expectedInstrument = expectedPosition.value("instrumentCode").toString();
        const auto expectedQuantity = expectedPosition.value("quantityText").toString();
        bool found = false;
        for (const auto& actualValue : actualPositions) {
            const auto actualPosition = actualValue.toObject();
            if (actualPosition.value("accountId").toString() != expectedAccount
                || actualPosition.value("instrumentCode").toString() != expectedInstrument) {
                continue;
            }
            found = true;
            if (actualPosition.value("portfolioId").toString() != "PF-DEMO-001") {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx008MultiAccount,
                    "FX008 must preserve portfolioId on each account-scoped position.",
                    false,
                    {"positionListResponseRaw.positions.portfolioId"});
            }
            if (actualPosition.value("quantityText").toString() != expectedQuantity) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx008MultiAccount,
                    "FX008 quantityText must match each account-scoped expected position.",
                    false,
                    {"positionListResponseRaw.positions.quantityText"});
            }
            if (actualPosition.value("costAmountText").toString().isEmpty()) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx008MultiAccount,
                    "FX008 must provide costAmountText per account-scoped position.",
                    false,
                    {"positionListResponseRaw.positions.costAmountText"});
            }
            const auto marketValue = actualPosition.value("marketValueText").toString();
            const auto unrealizedPnl = actualPosition.value("unrealizedPnlText").toString();
            if ((!marketValue.isEmpty() && marketValue != "unavailable")
                || (!unrealizedPnl.isEmpty() && unrealizedPnl != "unavailable")) {
                return makeResult(
                    fixture.fixtureId,
                    false,
                    kAssertionFailInvalidFx008MultiAccount,
                    "FX008 must not fabricate market value or unrealized PnL.",
                    false,
                    {"marketValueText", "unrealizedPnlText"});
            }
            if (expectedAccount == "ACC-DEMO-001") {
                sawAccountOne = true;
            }
            if (expectedAccount == "ACC-DEMO-002") {
                sawAccountTwo = true;
            }
        }
        if (!found) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx008MultiAccount,
                "FX008 positions must contain every expected accountId and instrumentCode pair.",
                false,
                {"positionListResponseRaw.positions.accountId", "positionListResponseRaw.positions.instrumentCode"});
        }
    }

    if (!sawAccountOne || !sawAccountTwo) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 must keep ACC-DEMO-001 and ACC-DEMO-002 separated.",
            false,
            {"positionListResponseRaw.positions.accountId"});
    }

    const auto accountCashSummaries = result.cashSummaryRaw.value("accountCashSummaries");
    if (!accountCashSummaries.isArray() || accountCashSummaries.toArray().size() < 2) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 cashSummaryRaw must expose accountCashSummaries instead of a mixed account balance.",
            false,
            {"cashSummaryRaw.accountCashSummaries"});
    }

    bool sawAccountOneCash = false;
    bool sawAccountTwoCash = false;
    for (const auto& summaryValue : accountCashSummaries.toArray()) {
        const auto summary = summaryValue.toObject();
        if (summary.value("accountId").toString() == "ACC-DEMO-001") {
            sawAccountOneCash = summary.value("cashBalanceText").toString() == "48999.00 CNY";
        }
        if (summary.value("accountId").toString() == "ACC-DEMO-002") {
            sawAccountTwoCash = summary.value("cashBalanceText").toString() == "47999.00 CNY";
        }
    }
    if (!sawAccountOneCash || !sawAccountTwoCash) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 account cash summaries must keep account balances separated.",
            false,
            {"cashSummaryRaw.accountCashSummaries.cashBalanceText"});
    }

    if (result.portfolioPnlRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx008MultiAccount,
            "FX008 must include portfolioPnlRaw without fabricated valuation fields.",
            false,
            {"portfolioPnlRaw"});
    }

    if (expectsMissingPrice) {
        bool hasMarketPriceMissing = false;
        bool blockingMatches = false;
        for (const auto& issue : result.issues) {
            if (issue.code == "MARKET_PRICE_MISSING") {
                hasMarketPriceMissing = true;
                blockingMatches = issue.blocking == expectedIssue->blocking;
            }
        }
        if (!hasMarketPriceMissing || !blockingMatches) {
            return makeResult(
                fixture.fixtureId,
                false,
                kAssertionFailInvalidFx008MultiAccount,
                "FX008 must contain MARKET_PRICE_MISSING with blocking matching the fixture expected issue.",
                false,
                {"issues.MARKET_PRICE_MISSING"});
        }
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx008MultiAccount,
        "FX008 multi-account replay result keeps account and cash dimensions separated.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "positionListResponseRaw.positions.accountId",
            "positionListResponseRaw.positions.portfolioId",
            "positionListResponseRaw.positions.instrumentCode",
            "positionListResponseRaw.positions.quantityText",
            "positionListResponseRaw.positions.costAmountText",
            "cashSummaryRaw.accountCashSummaries",
            "portfolioPnlRaw",
            "issues.MARKET_PRICE_MISSING",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx009BasePositionLockedResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX009_BASE_POSITION_LOCKED") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 base-position assertion only accepts FX009_BASE_POSITION_LOCKED.",
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

    const std::string expectedStatus = fixture.expectedIssues.empty() ? kReplayStatusOk : kReplayStatusWarning;
    if (!result.implemented || !result.replayExecuted || result.status != expectedStatus) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 replay result must be implemented=true, replayExecuted=true, and match expected status.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto expectedBaseValue = fixture.expectedOutputsRawJson.value("basePosition");
    if (!expectedBaseValue.isObject()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 fixture expected output must contain basePosition.",
            false,
            {"expectedOutputs.basePosition"});
    }
    const auto expectedBase = expectedBaseValue.toObject();
    if (result.basePositionRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 must produce basePositionRaw.",
            false,
            {"basePositionRaw"});
    }

    const auto checkBaseField = [&](const char* field, const QString& expected) -> bool {
        return result.basePositionRaw.value(QString::fromUtf8(field)).toString() == expected;
    };
    if (!checkBaseField("portfolioId", expectedBase.value("portfolioId").toString())
        || !checkBaseField("targetBaseRatioText", expectedBase.value("targetBaseRatioText").toString())
        || !checkBaseField("targetBaseAmountText", expectedBase.value("targetBaseAmountText").toString())
        || !checkBaseField("currentBaseRatioText", expectedBase.value("currentBaseRatioText").toString())
        || !checkBaseField("lockedBaseAmountText", expectedBase.value("lockedBaseAmountText").toString())
        || !checkBaseField("sellableAboveBaseAmountText", expectedBase.value("sellableAboveBaseAmountText").toString())
        || !checkBaseField("basePositionStatus", expectedBase.value("basePositionStatus").toString())
        || !checkBaseField("dataQualityStatus", expectedBase.value("dataQualityStatus").toString())) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 basePositionRaw must match fixture expected base-position fields.",
            false,
            {
                "basePositionRaw.portfolioId",
                "basePositionRaw.targetBaseRatioText",
                "basePositionRaw.targetBaseAmountText",
                "basePositionRaw.currentBaseRatioText",
                "basePositionRaw.lockedBaseAmountText",
                "basePositionRaw.sellableAboveBaseAmountText",
                "basePositionRaw.basePositionStatus",
                "basePositionRaw.dataQualityStatus",
            });
    }

    if (result.basePositionRaw.value("currentBaseAmountText").toString() != "20000.00 CNY"
        || result.basePositionRaw.value("damagedBaseAmountText").toString() != "0.00 CNY") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 must expose current base amount and zero damaged base amount for the locked sample.",
            false,
            {"basePositionRaw.currentBaseAmountText", "basePositionRaw.damagedBaseAmountText"});
    }

    if (!result.sniperPoolRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 must not produce sniperPoolRaw.",
            false,
            {"sniperPoolRaw"});
    }

    if (result.basePositionRaw.contains("tradeDraft") || result.basePositionRaw.contains("sellAction")
        || result.basePositionRaw.contains("recommendedAction")) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx009BasePositionLocked,
            "FX009 must not expose TradeDraft, sell action, or trading recommendation fields.",
            false,
            {"basePositionRaw.tradeDraft", "basePositionRaw.sellAction", "basePositionRaw.recommendedAction"});
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx009BasePositionLocked,
        "FX009 base-position replay result exposes readonly locked-base fields without actions.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "basePositionRaw.targetBaseRatioText",
            "basePositionRaw.targetBaseAmountText",
            "basePositionRaw.currentBaseAmountText",
            "basePositionRaw.lockedBaseAmountText",
            "basePositionRaw.sellableAboveBaseAmountText",
            "basePositionRaw.basePositionStatus",
            "sniperPoolRaw",
            "tradeDraft",
            "sellAction",
        });
}

AccountingAssertionResult AccountingReplayAssertionSkeleton::assertFx010SniperTierCompletedResult(
    const AccountingFixture& fixture,
    const AccountingReplayResult& result) const
{
    if (fixture.fixtureId != "FX010_SNIPER_TIER_COMPLETED") {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 sniper-tier assertion only accepts FX010_SNIPER_TIER_COMPLETED.",
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

    const std::string expectedStatus = fixture.expectedIssues.empty() ? kReplayStatusOk : kReplayStatusWarning;
    if (!result.implemented || !result.replayExecuted || result.status != expectedStatus) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 replay result must be implemented=true, replayExecuted=true, and match expected status.",
            false,
            {"implemented", "replayExecuted", "status"});
    }

    const auto expectedSniperValue = fixture.expectedOutputsRawJson.value("sniperPool");
    if (!expectedSniperValue.isObject()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 fixture expected output must contain sniperPool.",
            false,
            {"expectedOutputs.sniperPool"});
    }
    const auto expectedSniper = expectedSniperValue.toObject();
    if (result.sniperPoolRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 must produce sniperPoolRaw.",
            false,
            {"sniperPoolRaw"});
    }

    const auto checkSniperField = [&](const char* field, const QString& expected) -> bool {
        return result.sniperPoolRaw.value(QString::fromUtf8(field)).toString() == expected;
    };
    if (!checkSniperField("portfolioId", expectedSniper.value("portfolioId").toString())
        || !checkSniperField("poolAmountText", expectedSniper.value("poolAmountText").toString())
        || !checkSniperField("usedAmountText", expectedSniper.value("usedAmountText").toString())
        || !checkSniperField("remainingAmountText", expectedSniper.value("remainingAmountText").toString())
        || result.sniperPoolRaw.value("tierCount").toInt() != expectedSniper.value("tierCount").toInt()
        || !checkSniperField("dataQualityStatus", expectedSniper.value("dataQualityStatus").toString())) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 sniperPoolRaw must match fixture expected pool fields.",
            false,
            {
                "sniperPoolRaw.portfolioId",
                "sniperPoolRaw.poolAmountText",
                "sniperPoolRaw.usedAmountText",
                "sniperPoolRaw.remainingAmountText",
                "sniperPoolRaw.tierCount",
                "sniperPoolRaw.dataQualityStatus",
            });
    }

    if (result.sniperPoolRaw.value("calculationVersion").toString().isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 sniperPoolRaw must expose a stable calculationVersion.",
            false,
            {"sniperPoolRaw.calculationVersion"});
    }

    const auto expectedTiersValue = expectedSniper.value("tierSummary");
    const auto actualTiersValue = result.sniperPoolRaw.value("tierSummary");
    if (!expectedTiersValue.isArray() || !actualTiersValue.isArray()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 must expose tierSummary arrays in expected output and result.",
            false,
            {"expectedOutputs.sniperPool.tierSummary", "sniperPoolRaw.tierSummary"});
    }

    QJsonObject expectedT1;
    for (const auto& value : expectedTiersValue.toArray()) {
        const auto tier = value.toObject();
        if (tier.value("tierName").toString() == "T1") {
            expectedT1 = tier;
        }
    }
    QJsonObject actualT1;
    for (const auto& value : actualTiersValue.toArray()) {
        const auto tier = value.toObject();
        if (tier.value("tierName").toString() == "T1") {
            actualT1 = tier;
        }
    }
    if (expectedT1.isEmpty() || actualT1.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 tierSummary must contain T1.",
            false,
            {"sniperPoolRaw.tierSummary.T1"});
    }

    if (actualT1.value("weight").toInt() != expectedT1.value("weight").toInt()
        || actualT1.value("targetAmountText").toString() != expectedT1.value("targetAmountText").toString()
        || actualT1.value("executedAmountText").toString() != expectedT1.value("executedAmountText").toString()
        || actualT1.value("remainingAmountText").toString() != expectedT1.value("remainingAmountText").toString()
        || actualT1.value("completed").toBool(false) != true
        || actualT1.value("dataQualityStatus").toString() != expectedT1.value("dataQualityStatus").toString()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 T1 tier output must match expected target, executed, remaining, and completed fields.",
            false,
            {
                "sniperPoolRaw.tierSummary.T1.weight",
                "sniperPoolRaw.tierSummary.T1.targetAmountText",
                "sniperPoolRaw.tierSummary.T1.executedAmountText",
                "sniperPoolRaw.tierSummary.T1.remainingAmountText",
                "sniperPoolRaw.tierSummary.T1.completed",
            });
    }

    if (!result.basePositionRaw.isEmpty()) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 must not produce basePositionRaw.",
            false,
            {"basePositionRaw"});
    }

    if (result.sniperPoolRaw.contains("tradeDraft") || result.sniperPoolRaw.contains("buyAction")
        || result.sniperPoolRaw.contains("sellAction") || result.sniperPoolRaw.contains("recommendedAction")) {
        return makeResult(
            fixture.fixtureId,
            false,
            kAssertionFailInvalidFx010SniperTierCompleted,
            "FX010 must not expose TradeDraft, buy action, sell action, or trading recommendation fields.",
            false,
            {
                "sniperPoolRaw.tradeDraft",
                "sniperPoolRaw.buyAction",
                "sniperPoolRaw.sellAction",
                "sniperPoolRaw.recommendedAction",
            });
    }

    return makeResult(
        fixture.fixtureId,
        true,
        kAssertionPassFx010SniperTierCompleted,
        "FX010 sniper-tier replay result exposes readonly sniper pool tier fields without actions.",
        false,
        {
            "fixtureId",
            "implemented",
            "replayExecuted",
            "status",
            "sniperPoolRaw.poolAmountText",
            "sniperPoolRaw.usedAmountText",
            "sniperPoolRaw.remainingAmountText",
            "sniperPoolRaw.tierCount",
            "sniperPoolRaw.tierSummary.T1",
            "sniperPoolRaw.calculationVersion",
            "basePositionRaw",
            "tradeDraft",
            "buyAction",
            "sellAction",
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
