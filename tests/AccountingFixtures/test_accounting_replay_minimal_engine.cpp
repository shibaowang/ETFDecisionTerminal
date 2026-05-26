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
using etfdt::tests::accounting::kAssertionPassFx002SingleBuy;
using etfdt::tests::accounting::kAssertionPassFx003BuySellPartial;
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
    const auto fx003 = requireValue(harness.fixtureByIdForTest("FX003_BUY_SELL_PARTIAL"), "FX003 fixture exists");
    const auto fx004 = requireValue(harness.fixtureByIdForTest("FX004_SELL_EXCEEDS_POSITION"), "FX004 fixture exists");

    AccountingReplayMinimalEngine engine;
    require(engine.supportsFixture("FX001_EMPTY_LEDGER"), "minimal engine supports FX001");
    require(engine.supportsFixture("FX002_SINGLE_BUY"), "minimal engine supports FX002");
    require(engine.supportsFixture("FX003_BUY_SELL_PARTIAL"), "minimal engine supports FX003");
    require(!engine.supportsFixture("FX004_SELL_EXCEEDS_POSITION"), "minimal engine does not support FX004");

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

        require(result.status == kReplayStatusNotImplemented, fixtureId + " remains NOT_IMPLEMENTED");
        require(!result.implemented, fixtureId + " remains implemented=false");
        require(!result.replayExecuted, fixtureId + " remains replayExecuted=false");
        const auto assertion = assertions.assertCurrentStubResult(fixture, result);
        require(assertion.passed, fixtureId + " still passes NOT_IMPLEMENTED guard assertion");
        require(assertion.status == kAssertionPassNotImplementedGuard, fixtureId + " guard status is PASS_NOT_IMPLEMENTED_GUARD");
    }

    std::cout << "Accounting replay minimal FX001/FX002/FX003 tests passed\n";
    return 0;
}
