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
using etfdt::tests::accounting::hasEmptyReplayOutputs;
using etfdt::tests::accounting::hasPortfolioPnl;
using etfdt::tests::accounting::hasPositionListResponse;
using etfdt::tests::accounting::hasReplayIssueCode;
using etfdt::tests::accounting::kAssertionPassFx001EmptyLedger;
using etfdt::tests::accounting::kAssertionPassFx002SingleBuy;
using etfdt::tests::accounting::kAssertionPassFx003BuySellPartial;
using etfdt::tests::accounting::kAssertionPassFx004SellExceedsPosition;
using etfdt::tests::accounting::kAssertionPassFx005MissingFee;
using etfdt::tests::accounting::kAssertionPassFx006NegativeCash;
using etfdt::tests::accounting::kAssertionPassFx007MultiInstrument;
using etfdt::tests::accounting::kAssertionPassFx008MultiAccount;
using etfdt::tests::accounting::kAssertionPassNotImplementedGuard;
using etfdt::tests::accounting::kReplayStatusError;
using etfdt::tests::accounting::kReplayStatusInvalidFixture;
using etfdt::tests::accounting::kReplayStatusNotImplemented;
using etfdt::tests::accounting::kReplayStatusOk;
using etfdt::tests::accounting::kReplayStatusWarning;

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

bool hasBlockingIssueCode(const AccountingReplayResult& result, const std::string& code)
{
    for (const auto& issue : result.issues) {
        if (issue.code == code && issue.blocking) {
            return true;
        }
    }
    return false;
}

bool issueBlockingMatches(const AccountingReplayResult& result, const std::string& code, bool expectedBlocking)
{
    for (const auto& issue : result.issues) {
        if (issue.code == code) {
            return issue.blocking == expectedBlocking;
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
    const auto fx003 = requireValue(harness.fixtureByIdForTest("FX003_BUY_SELL_PARTIAL"), "FX003 fixture exists");
    const auto fx004 = requireValue(harness.fixtureByIdForTest("FX004_SELL_EXCEEDS_POSITION"), "FX004 fixture exists");
    const auto fx005 = requireValue(harness.fixtureByIdForTest("FX005_MISSING_FEE"), "FX005 fixture exists");
    const auto fx006 = requireValue(harness.fixtureByIdForTest("FX006_NEGATIVE_CASH"), "FX006 fixture exists");
    const auto fx007 = requireValue(harness.fixtureByIdForTest("FX007_MULTI_INSTRUMENT"), "FX007 fixture exists");
    const auto fx008 = requireValue(harness.fixtureByIdForTest("FX008_MULTI_ACCOUNT"), "FX008 fixture exists");

    AccountingReplayMinimalEngine engine;
    require(engine.supportsFixture("FX001_EMPTY_LEDGER"), "minimal engine supports FX001");
    require(engine.supportsFixture("FX002_SINGLE_BUY"), "minimal engine supports FX002");
    require(engine.supportsFixture("FX003_BUY_SELL_PARTIAL"), "minimal engine supports FX003");
    require(engine.supportsFixture("FX004_SELL_EXCEEDS_POSITION"), "minimal engine supports FX004");
    require(engine.supportsFixture("FX005_MISSING_FEE"), "minimal engine supports FX005");
    require(engine.supportsFixture("FX006_NEGATIVE_CASH"), "minimal engine supports FX006");
    require(engine.supportsFixture("FX007_MULTI_INSTRUMENT"), "minimal engine supports FX007");
    require(engine.supportsFixture("FX008_MULTI_ACCOUNT"), "minimal engine supports FX008");
    require(!engine.supportsFixture("FX009_BASE_POSITION_LOCKED"), "minimal engine does not support FX009");

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
    require(fx002Direct.implemented, "FX002 direct replay implemented=true");
    require(fx002Direct.replayExecuted, "FX002 direct replayExecuted=true");
    require(fx002Direct.status == kReplayStatusOk, "FX002 direct status=OK");
    require(hasPositionListResponse(fx002Direct), "FX002 has positionListResponseRaw");
    require(fx002Direct.positionListResponseRaw.value("positions").isArray(), "FX002 positions is an array");
    require(fx002Direct.positionListResponseRaw.value("positions").toArray().size() == 1, "FX002 positions array has one row");
    const auto fx002Position = fx002Direct.positionListResponseRaw.value("positions").toArray().first().toObject();
    require(fx002Position.value("instrumentCode").toString() == "159509", "FX002 instrumentCode is 159509");
    require(fx002Position.value("quantityText").toString() == "1000", "FX002 quantityText is 1000");
    require(fx002Position.value("costAmountText").toString() == "1001.00 CNY", "FX002 costAmountText includes fee");
    require(hasCashSummary(fx002Direct), "FX002 has cashSummaryRaw");
    require(fx002Direct.cashSummaryRaw.value("cashBalanceText").toString() == "98999.00 CNY", "FX002 cashBalanceText is 98999.00 CNY");
    require(hasPortfolioPnl(fx002Direct), "FX002 has portfolioPnlRaw");
    require(!hasBlockingIssue(fx002Direct), "FX002 has no blocking issue");

    const auto fx003Direct = engine.replayFixture(fx003);
    require(fx003Direct.implemented, "FX003 direct replay implemented=true");
    require(fx003Direct.replayExecuted, "FX003 direct replayExecuted=true");
    require(fx003Direct.status == kReplayStatusOk, "FX003 direct status=OK");
    require(hasPositionListResponse(fx003Direct), "FX003 has positionListResponseRaw");
    require(fx003Direct.positionListResponseRaw.value("positions").isArray(), "FX003 positions is an array");
    require(fx003Direct.positionListResponseRaw.value("positions").toArray().size() == 1, "FX003 positions array has one row");
    const auto fx003Position = fx003Direct.positionListResponseRaw.value("positions").toArray().first().toObject();
    require(fx003Position.value("instrumentCode").toString() == "159509", "FX003 instrumentCode is 159509");
    require(fx003Position.value("quantityText").toString() == "600", "FX003 quantityText is 600");
    require(fx003Position.value("costAmountText").toString() == "600.60 CNY", "FX003 costAmountText is 600.60 CNY");
    require(hasCashSummary(fx003Direct), "FX003 has cashSummaryRaw");
    require(fx003Direct.cashSummaryRaw.value("cashBalanceText").toString() == "99478.00 CNY", "FX003 cashBalanceText is 99478.00 CNY");
    require(fx003Direct.cashSummaryRaw.value("sellCashInflowText").toString() == "479.00 CNY", "FX003 sellCashInflowText is 479.00 CNY");
    require(fx003Direct.cashSummaryRaw.value("feeTotalText").toString() == "2.00 CNY", "FX003 feeTotalText is 2.00 CNY");
    require(hasPortfolioPnl(fx003Direct), "FX003 has portfolioPnlRaw");
    require(fx003Direct.portfolioPnlRaw.value("realizedPnlText").toString() == "78.60 CNY", "FX003 realizedPnlText is 78.60 CNY");
    require(!hasBlockingIssue(fx003Direct), "FX003 has no blocking issue");

    const auto fx004Direct = engine.replayFixture(fx004);
    require(fx004Direct.implemented, "FX004 direct replay implemented=true");
    require(fx004Direct.replayExecuted, "FX004 direct replayExecuted=true");
    require(fx004Direct.status == kReplayStatusError, "FX004 direct status=ERROR");
    require(hasReplayIssueCode(fx004Direct, "SELL_EXCEEDS_POSITION"), "FX004 issues contain SELL_EXCEEDS_POSITION");
    require(hasBlockingIssueCode(fx004Direct, "SELL_EXCEEDS_POSITION"), "FX004 SELL_EXCEEDS_POSITION is blocking");
    require(hasEmptyReplayOutputs(fx004Direct), "FX004 does not generate normal raw outputs");
    require(fx004Direct.metadata.fixtureBlockingExpected, "FX004 keeps blocking expected metadata");

    const auto fx005Direct = engine.replayFixture(fx005);
    require(fx005Direct.implemented, "FX005 direct replay implemented=true");
    require(fx005Direct.replayExecuted, "FX005 direct replayExecuted=true");
    require(fx005Direct.status == kReplayStatusWarning, "FX005 direct status=WARNING");
    require(hasReplayIssueCode(fx005Direct, "MISSING_FEE"), "FX005 issues contain MISSING_FEE");
    require(issueBlockingMatches(fx005Direct, "MISSING_FEE", false), "FX005 MISSING_FEE blocking matches fixture");
    require(!hasBlockingIssue(fx005Direct), "FX005 has no blocking issue");
    require(hasEmptyReplayOutputs(fx005Direct), "FX005 does not generate normal raw outputs");

    const auto fx006Direct = engine.replayFixture(fx006);
    require(fx006Direct.implemented, "FX006 direct replay implemented=true");
    require(fx006Direct.replayExecuted, "FX006 direct replayExecuted=true");
    require(fx006Direct.status == kReplayStatusError, "FX006 direct status=ERROR");
    require(hasReplayIssueCode(fx006Direct, "NEGATIVE_CASH"), "FX006 issues contain NEGATIVE_CASH");
    require(hasBlockingIssueCode(fx006Direct, "NEGATIVE_CASH"), "FX006 NEGATIVE_CASH is blocking");
    require(hasEmptyReplayOutputs(fx006Direct), "FX006 does not generate normal raw outputs");

    const auto fx007Direct = engine.replayFixture(fx007);
    require(fx007Direct.implemented, "FX007 direct replay implemented=true");
    require(fx007Direct.replayExecuted, "FX007 direct replayExecuted=true");
    require(fx007Direct.status == kReplayStatusWarning, "FX007 direct status=WARNING");
    require(hasPositionListResponse(fx007Direct), "FX007 has positionListResponseRaw");
    require(fx007Direct.positionListResponseRaw.value("positions").isArray(), "FX007 positions is an array");
    require(fx007Direct.positionListResponseRaw.value("positions").toArray().size() == 2, "FX007 positions array has two rows");
    bool has159509 = false;
    bool has518880 = false;
    for (const auto& value : fx007Direct.positionListResponseRaw.value("positions").toArray()) {
        const auto position = value.toObject();
        const auto instrument = position.value("instrumentCode").toString();
        require(
            position.value("marketValueText").toString() == "unavailable",
            "FX007 does not fabricate marketValueText");
        require(
            position.value("unrealizedPnlText").toString() == "unavailable",
            "FX007 does not fabricate unrealizedPnlText");
        if (instrument == "159509") {
            has159509 = true;
            require(position.value("quantityText").toString() == "1000", "FX007 159509 quantityText is 1000");
            require(position.value("costAmountText").toString() == "1001.00 CNY", "FX007 159509 costAmountText is 1001.00 CNY");
        }
        if (instrument == "518880") {
            has518880 = true;
            require(position.value("quantityText").toString() == "500", "FX007 518880 quantityText is 500");
            require(position.value("costAmountText").toString() == "2001.00 CNY", "FX007 518880 costAmountText is 2001.00 CNY");
        }
    }
    require(has159509, "FX007 positions include 159509");
    require(has518880, "FX007 positions include 518880");
    require(hasCashSummary(fx007Direct), "FX007 has cashSummaryRaw");
    require(fx007Direct.cashSummaryRaw.value("cashBalanceText").toString() == "96998.00 CNY", "FX007 cashBalanceText is 96998.00 CNY");
    require(hasPortfolioPnl(fx007Direct), "FX007 has portfolioPnlRaw");
    require(fx007Direct.portfolioPnlRaw.value("totalAssetsText").toString().isEmpty(), "FX007 totalAssetsText remains empty");
    require(hasReplayIssueCode(fx007Direct, "MARKET_PRICE_MISSING"), "FX007 issues contain MARKET_PRICE_MISSING");
    require(issueBlockingMatches(fx007Direct, "MARKET_PRICE_MISSING", false), "FX007 MARKET_PRICE_MISSING is non-blocking");

    const auto fx008Direct = engine.replayFixture(fx008);
    require(fx008Direct.implemented, "FX008 direct replay implemented=true");
    require(fx008Direct.replayExecuted, "FX008 direct replayExecuted=true");
    require(fx008Direct.status == kReplayStatusOk, "FX008 direct status=OK");
    require(hasPositionListResponse(fx008Direct), "FX008 has positionListResponseRaw");
    require(fx008Direct.positionListResponseRaw.value("positions").isArray(), "FX008 positions is an array");
    require(fx008Direct.positionListResponseRaw.value("positions").toArray().size() == 2, "FX008 positions array has two rows");
    bool hasAccountOnePosition = false;
    bool hasAccountTwoPosition = false;
    for (const auto& value : fx008Direct.positionListResponseRaw.value("positions").toArray()) {
        const auto position = value.toObject();
        require(position.value("instrumentCode").toString() == "159509", "FX008 instrumentCode remains 159509");
        require(position.value("portfolioId").toString() == "PF-DEMO-001", "FX008 preserves portfolioId");
        require(position.value("marketValueText").toString() == "unavailable", "FX008 does not fabricate marketValueText");
        require(position.value("unrealizedPnlText").toString() == "unavailable", "FX008 does not fabricate unrealizedPnlText");
        require(position.value("quantityText").toString() != "3000", "FX008 does not merge account quantities");
        if (position.value("accountId").toString() == "ACC-DEMO-001") {
            hasAccountOnePosition = true;
            require(position.value("quantityText").toString() == "1000", "FX008 account one quantityText is 1000");
            require(position.value("costAmountText").toString() == "1001.00 CNY", "FX008 account one costAmountText is 1001.00 CNY");
        }
        if (position.value("accountId").toString() == "ACC-DEMO-002") {
            hasAccountTwoPosition = true;
            require(position.value("quantityText").toString() == "2000", "FX008 account two quantityText is 2000");
            require(position.value("costAmountText").toString() == "2001.00 CNY", "FX008 account two costAmountText is 2001.00 CNY");
        }
    }
    require(hasAccountOnePosition, "FX008 positions include ACC-DEMO-001");
    require(hasAccountTwoPosition, "FX008 positions include ACC-DEMO-002");
    require(hasCashSummary(fx008Direct), "FX008 has cashSummaryRaw");
    require(fx008Direct.cashSummaryRaw.value("accountCashSummaries").isArray(), "FX008 cashSummaryRaw has accountCashSummaries");
    bool hasAccountOneCash = false;
    bool hasAccountTwoCash = false;
    for (const auto& value : fx008Direct.cashSummaryRaw.value("accountCashSummaries").toArray()) {
        const auto cash = value.toObject();
        require(cash.value("portfolioId").toString() == "PF-DEMO-001", "FX008 account cash preserves portfolioId");
        if (cash.value("accountId").toString() == "ACC-DEMO-001") {
            hasAccountOneCash = true;
            require(cash.value("cashBalanceText").toString() == "48999.00 CNY", "FX008 account one cashBalanceText is 48999.00 CNY");
        }
        if (cash.value("accountId").toString() == "ACC-DEMO-002") {
            hasAccountTwoCash = true;
            require(cash.value("cashBalanceText").toString() == "47999.00 CNY", "FX008 account two cashBalanceText is 47999.00 CNY");
        }
    }
    require(hasAccountOneCash, "FX008 cash summaries include ACC-DEMO-001");
    require(hasAccountTwoCash, "FX008 cash summaries include ACC-DEMO-002");
    require(hasPortfolioPnl(fx008Direct), "FX008 has portfolioPnlRaw");
    require(fx008Direct.portfolioPnlRaw.value("marketValueText").toString() == "unavailable", "FX008 portfolio PnL does not fabricate marketValueText");
    require(fx008Direct.portfolioPnlRaw.value("unrealizedPnlText").toString() == "unavailable", "FX008 portfolio PnL does not fabricate unrealizedPnlText");
    require(!hasReplayIssueCode(fx008Direct, "MARKET_PRICE_MISSING"), "FX008 does not report missing market price when fixture provides one");
    require(!hasBlockingIssue(fx008Direct), "FX008 has no blocking issue");

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

    const auto fx002Harness = requireResult(harness, "FX002_SINGLE_BUY");
    require(fx002Harness.status == kReplayStatusOk, "minimal harness FX002 status=OK");
    require(fx002Harness.implemented, "minimal harness FX002 implemented=true");
    require(fx002Harness.replayExecuted, "minimal harness FX002 replayExecuted=true");
    const auto fx002Assertion = assertions.assertFx002SingleBuyResult(fx002, fx002Harness);
    require(fx002Assertion.passed, "FX002 single-buy assertion passes");
    require(fx002Assertion.status == kAssertionPassFx002SingleBuy, "FX002 assertion status is PASS_FX002_SINGLE_BUY");

    const auto fx003Harness = requireResult(harness, "FX003_BUY_SELL_PARTIAL");
    require(fx003Harness.status == kReplayStatusOk, "minimal harness FX003 status=OK");
    require(fx003Harness.implemented, "minimal harness FX003 implemented=true");
    require(fx003Harness.replayExecuted, "minimal harness FX003 replayExecuted=true");
    const auto fx003Assertion = assertions.assertFx003BuySellPartialResult(fx003, fx003Harness);
    require(fx003Assertion.passed, "FX003 buy-sell partial assertion passes");
    require(fx003Assertion.status == kAssertionPassFx003BuySellPartial, "FX003 assertion status is PASS_FX003_BUY_SELL_PARTIAL");

    const auto fx004Harness = requireResult(harness, "FX004_SELL_EXCEEDS_POSITION");
    require(fx004Harness.status == kReplayStatusError, "minimal harness FX004 status=ERROR");
    require(fx004Harness.implemented, "minimal harness FX004 implemented=true");
    require(fx004Harness.replayExecuted, "minimal harness FX004 replayExecuted=true");
    require(hasBlockingIssueCode(fx004Harness, "SELL_EXCEEDS_POSITION"), "minimal harness FX004 has blocking SELL_EXCEEDS_POSITION");
    require(hasEmptyReplayOutputs(fx004Harness), "minimal harness FX004 keeps raw outputs empty");
    const auto fx004Assertion = assertions.assertFx004SellExceedsPositionResult(fx004, fx004Harness);
    require(fx004Assertion.passed, "FX004 sell-exceeds-position assertion passes");
    require(fx004Assertion.status == kAssertionPassFx004SellExceedsPosition, "FX004 assertion status is PASS_FX004_SELL_EXCEEDS_POSITION");

    const auto fx005Harness = requireResult(harness, "FX005_MISSING_FEE");
    require(fx005Harness.status == kReplayStatusWarning, "minimal harness FX005 status=WARNING");
    require(fx005Harness.implemented, "minimal harness FX005 implemented=true");
    require(fx005Harness.replayExecuted, "minimal harness FX005 replayExecuted=true");
    require(issueBlockingMatches(fx005Harness, "MISSING_FEE", false), "minimal harness FX005 has non-blocking MISSING_FEE");
    require(hasEmptyReplayOutputs(fx005Harness), "minimal harness FX005 keeps raw outputs empty");
    const auto fx005Assertion = assertions.assertFx005MissingFeeResult(fx005, fx005Harness);
    require(fx005Assertion.passed, "FX005 missing-fee assertion passes");
    require(fx005Assertion.status == kAssertionPassFx005MissingFee, "FX005 assertion status is PASS_FX005_MISSING_FEE");

    const auto fx006Harness = requireResult(harness, "FX006_NEGATIVE_CASH");
    require(fx006Harness.status == kReplayStatusError, "minimal harness FX006 status=ERROR");
    require(fx006Harness.implemented, "minimal harness FX006 implemented=true");
    require(fx006Harness.replayExecuted, "minimal harness FX006 replayExecuted=true");
    require(hasBlockingIssueCode(fx006Harness, "NEGATIVE_CASH"), "minimal harness FX006 has blocking NEGATIVE_CASH");
    require(hasEmptyReplayOutputs(fx006Harness), "minimal harness FX006 keeps raw outputs empty");
    const auto fx006Assertion = assertions.assertFx006NegativeCashResult(fx006, fx006Harness);
    require(fx006Assertion.passed, "FX006 negative-cash assertion passes");
    require(fx006Assertion.status == kAssertionPassFx006NegativeCash, "FX006 assertion status is PASS_FX006_NEGATIVE_CASH");

    const auto fx007Harness = requireResult(harness, "FX007_MULTI_INSTRUMENT");
    require(fx007Harness.status == kReplayStatusWarning, "minimal harness FX007 status=WARNING");
    require(fx007Harness.implemented, "minimal harness FX007 implemented=true");
    require(fx007Harness.replayExecuted, "minimal harness FX007 replayExecuted=true");
    require(issueBlockingMatches(fx007Harness, "MARKET_PRICE_MISSING", false), "minimal harness FX007 has non-blocking MARKET_PRICE_MISSING");
    const auto fx007Assertion = assertions.assertFx007MultiInstrumentResult(fx007, fx007Harness);
    require(fx007Assertion.passed, "FX007 multi-instrument assertion passes");
    require(fx007Assertion.status == kAssertionPassFx007MultiInstrument, "FX007 assertion status is PASS_FX007_MULTI_INSTRUMENT");

    const auto fx008Harness = requireResult(harness, "FX008_MULTI_ACCOUNT");
    require(fx008Harness.status == kReplayStatusOk, "minimal harness FX008 status=OK");
    require(fx008Harness.implemented, "minimal harness FX008 implemented=true");
    require(fx008Harness.replayExecuted, "minimal harness FX008 replayExecuted=true");
    require(!hasReplayIssueCode(fx008Harness, "MARKET_PRICE_MISSING"), "minimal harness FX008 has no missing market price issue");
    const auto fx008Assertion = assertions.assertFx008MultiAccountResult(fx008, fx008Harness);
    require(fx008Assertion.passed, "FX008 multi-account assertion passes");
    require(fx008Assertion.status == kAssertionPassFx008MultiAccount, "FX008 assertion status is PASS_FX008_MULTI_ACCOUNT");

    for (const auto& fixtureId : harness.coveredFixtureIds()) {
        const auto fixture = requireValue(harness.fixtureByIdForTest(fixtureId), "fixture exists for minimal result loop");
        const auto result = requireResult(harness, fixtureId);
        if (fixtureId == "FX001_EMPTY_LEDGER") {
            require(result.status == kReplayStatusOk, "FX001 remains OK in minimal loop");
            continue;
        }
        if (fixtureId == "FX002_SINGLE_BUY") {
            require(result.status == kReplayStatusOk, "FX002 remains OK in minimal loop");
            continue;
        }
        if (fixtureId == "FX003_BUY_SELL_PARTIAL") {
            require(result.status == kReplayStatusOk, "FX003 remains OK in minimal loop");
            continue;
        }
        if (fixtureId == "FX004_SELL_EXCEEDS_POSITION") {
            require(result.status == kReplayStatusError, "FX004 remains ERROR in minimal loop");
            require(hasBlockingIssueCode(result, "SELL_EXCEEDS_POSITION"), "FX004 keeps blocking SELL_EXCEEDS_POSITION in minimal loop");
            continue;
        }
        if (fixtureId == "FX005_MISSING_FEE") {
            require(result.status == kReplayStatusWarning, "FX005 remains WARNING in minimal loop");
            require(issueBlockingMatches(result, "MISSING_FEE", false), "FX005 keeps non-blocking MISSING_FEE in minimal loop");
            continue;
        }
        if (fixtureId == "FX006_NEGATIVE_CASH") {
            require(result.status == kReplayStatusError, "FX006 remains ERROR in minimal loop");
            require(hasBlockingIssueCode(result, "NEGATIVE_CASH"), "FX006 keeps blocking NEGATIVE_CASH in minimal loop");
            continue;
        }
        if (fixtureId == "FX007_MULTI_INSTRUMENT") {
            require(result.status == kReplayStatusWarning, "FX007 remains WARNING in minimal loop");
            require(issueBlockingMatches(result, "MARKET_PRICE_MISSING", false), "FX007 keeps non-blocking MARKET_PRICE_MISSING in minimal loop");
            continue;
        }
        if (fixtureId == "FX008_MULTI_ACCOUNT") {
            require(result.status == kReplayStatusOk, "FX008 remains OK in minimal loop");
            require(result.implemented, "FX008 remains implemented=true in minimal loop");
            require(result.replayExecuted, "FX008 remains replayExecuted=true in minimal loop");
            continue;
        }

        require(result.status == kReplayStatusNotImplemented, fixtureId + " remains NOT_IMPLEMENTED");
        require(!result.implemented, fixtureId + " remains implemented=false");
        require(!result.replayExecuted, fixtureId + " remains replayExecuted=false");
        const auto assertion = assertions.assertCurrentStubResult(fixture, result);
        require(assertion.passed, fixtureId + " still passes NOT_IMPLEMENTED guard assertion");
        require(assertion.status == kAssertionPassNotImplementedGuard, fixtureId + " guard status is PASS_NOT_IMPLEMENTED_GUARD");
    }

    std::cout << "Accounting replay minimal FX001/FX002/FX003/FX004/FX005/FX006/FX007/FX008 tests passed\n";
    return 0;
}
