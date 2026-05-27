#include "AccountingEngine/AccountingEngine.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool contains(const std::vector<std::string>& values, const std::string& expected)
{
    return std::find(values.begin(), values.end(), expected) != values.end();
}

int expect(bool condition, const std::string& message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "FAILED: " << message << '\n';
    return 1;
}

etfdt::accounting::ReplayRequestDto makeRequest()
{
    etfdt::accounting::ReplayRequestDto request;
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PORT-DEMO-001";
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    return request;
}

etfdt::accounting::CashFactDto makeInitialCash(const std::string& amountText = "100000.00",
                                               const std::string& currency = "CNY")
{
    return etfdt::accounting::CashFactDto{
        "CASH-001",
        "2026-01-01T00:00:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        etfdt::accounting::CashAction::InitialCash,
        amountText,
        currency,
        "sell exceeds position test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& instrumentCode = "159509",
                                        const std::string& quantityText = "1000",
                                        const std::string& amountText = "1000.00",
                                        const std::string& feeText = "1.00",
                                        const std::string& currency = "CNY")
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-BUY-001",
        "2026-01-02T09:30:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        instrumentCode,
        etfdt::accounting::TradeAction::Buy,
        quantityText,
        "1.0000",
        amountText,
        feeText,
        "-1001.00",
        currency,
        "unit-test",
        "sell exceeds position buy",
    };
}

etfdt::accounting::TradeFactDto makeSell(const std::string& instrumentCode = "159509",
                                         const std::string& quantityText = "1200",
                                         const std::string& amountText = "1440.00",
                                         const std::string& feeText = "1.00",
                                         const std::string& currency = "CNY")
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-SELL-001",
        "2026-01-03T09:30:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        instrumentCode,
        etfdt::accounting::TradeAction::Sell,
        quantityText,
        "1.2000",
        amountText,
        feeText,
        "1439.00",
        currency,
        "unit-test",
        "sell exceeds position sell",
    };
}

} // namespace

int main()
{
    using namespace etfdt::accounting;

    int failures = 0;

    AccountingReplayEngine engine;
    const auto request = makeRequest();

    const auto emptyResult = engine.replayReadOnly(request, {}, {}, {}, {});
    failures += expect(emptyResult.status == AccountingReplayStatus::Ok, "empty ledger should still pass");

    const auto singleBuyResult = engine.replayReadOnly(request, {makeBuy()}, {makeInitialCash()}, {}, {});
    failures += expect(singleBuyResult.status == AccountingReplayStatus::Ok, "single buy should still pass");

    const auto partialSellResult =
        engine.replayReadOnly(request, {makeBuy(), makeSell("159509", "400", "480.00")}, {makeInitialCash()}, {}, {});
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell should still pass");

    const auto sellExceedsResult = engine.replayReadOnly(request, {makeBuy(), makeSell()}, {makeInitialCash()}, {}, {});
    failures += expect(sellExceedsResult.implemented, "sell exceeds position should be implemented");
    failures += expect(sellExceedsResult.replayExecuted, "sell exceeds position should execute replay");
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Error, "sell exceeds position status should be ERROR");
    failures += expect(sellExceedsResult.message == "Sell quantity exceeds available position.",
                       "sell exceeds position message should be stable");
    failures += expect(sellExceedsResult.issues.size() == 1, "sell exceeds position should have one issue");
    if (!sellExceedsResult.issues.empty()) {
        const auto& issue = sellExceedsResult.issues.front();
        failures += expect(issue.code == AccountingIssueCode::SellExceedsPosition,
                           "sell exceeds position issue code should be SELL_EXCEEDS_POSITION");
        failures += expect(issue.level == AccountingIssueLevel::Error, "sell exceeds position issue level should be ERROR");
        failures += expect(issue.blocking, "sell exceeds position issue should be blocking");
    }
    failures += expect(sellExceedsResult.positionList.positions.empty(),
                       "sell exceeds position should not output success positions");
    failures += expect(!sellExceedsResult.hasCashSummary, "sell exceeds position should not output cash summary");
    failures += expect(!sellExceedsResult.hasPortfolioPnl, "sell exceeds position should not output portfolio PnL");
    failures += expect(!sellExceedsResult.hasBasePosition, "sell exceeds position should not output base position");
    failures += expect(!sellExceedsResult.hasSniperPool, "sell exceeds position should not output sniper pool");

    const auto equalSellResult =
        engine.replayReadOnly(request, {makeBuy(), makeSell("159509", "1000", "1200.00")}, {makeInitialCash()}, {}, {});
    failures += expect(equalSellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "sell quantity equal to buy quantity should not be classified as oversell");

    const auto onlySellResult = engine.replayReadOnly(request, {makeSell()}, {makeInitialCash()}, {}, {});
    failures += expect(onlySellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "only SELL fact should remain unsupported");

    const auto differentInstrumentResult =
        engine.replayReadOnly(request, {makeBuy("159509"), makeSell("518880")}, {makeInitialCash()}, {}, {});
    failures += expect(differentInstrumentResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "different instrument should remain unsupported");

    auto differentAccountSell = makeSell();
    differentAccountSell.accountId = "ACC-DEMO-002";
    const auto differentAccountResult =
        engine.replayReadOnly(request, {makeBuy(), differentAccountSell}, {makeInitialCash()}, {}, {});
    failures += expect(differentAccountResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "different account should remain unsupported");

    const auto unsupportedCurrencyResult =
        engine.replayReadOnly(request,
                              {makeBuy("159509", "1000", "1000.00", "1.00", "USD"),
                               makeSell("159509", "1200", "1440.00", "1.00", "USD")},
                              {makeInitialCash("100000.00", "USD")},
                              {},
                              {});
    failures += expect(unsupportedCurrencyResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "USD sell exceeds position should remain unsupported");

    auto missingSellFee = makeSell();
    missingSellFee.feeText.clear();
    const auto missingSellFeeResult =
        engine.replayReadOnly(request, {makeBuy(), missingSellFee}, {makeInitialCash()}, {}, {});
    failures += expect(missingSellFeeResult.status == AccountingReplayStatus::InvalidRequest,
                       "missing sell fee should return INVALID_REQUEST");

    const auto invalidQuantityResult =
        engine.replayReadOnly(request, {makeBuy(), makeSell("159509", "many")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidQuantityResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid quantity text should return INVALID_REQUEST");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.sellExceedsPositionDetectionSupported,
                       "sellExceedsPositionDetectionSupported should be true");
    failures += expect(!capabilities.writeEnabled, "capabilities.writeEnabled should remain false");
    failures += expect(!capabilities.snapshotWriteEnabled, "capabilities.snapshotWriteEnabled should remain false");
    failures += expect(!capabilities.tradeLogWriteEnabled, "capabilities.tradeLogWriteEnabled should remain false");

    const auto boundary = accountingEngineBoundary();
    failures += expect(contains(boundary.forbiddenDependencies, "DataAccess"),
                       "boundary forbiddenDependencies should still include DataAccess");
    failures += expect(contains(boundary.forbiddenDependencies, "tests/AccountingFixtures"),
                       "boundary forbiddenDependencies should still include tests/AccountingFixtures");
    failures += expect(contains(boundary.forbiddenWrites, "trade_log"),
                       "boundary forbiddenWrites should still include trade_log");
    failures += expect(contains(boundary.forbiddenWrites, "position_snapshot"),
                       "boundary forbiddenWrites should still include position_snapshot");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
