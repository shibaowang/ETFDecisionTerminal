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
        etfdt::accounting::RequestedReplayOutput::Pnl,
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
        "partial sell test",
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
        "partial sell buy",
    };
}

etfdt::accounting::TradeFactDto makeSell(const std::string& instrumentCode = "159509",
                                         const std::string& quantityText = "400",
                                         const std::string& amountText = "480.00",
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
        "479.00",
        currency,
        "unit-test",
        "partial sell sell",
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
        engine.replayReadOnly(request, {makeBuy(), makeSell()}, {makeInitialCash()}, {}, {});
    failures += expect(partialSellResult.implemented, "partial sell should be implemented");
    failures += expect(partialSellResult.replayExecuted, "partial sell should execute replay");
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell status should be OK");
    failures += expect(partialSellResult.issues.empty(), "partial sell should not contain issues");
    failures += expect(partialSellResult.positionList.positions.size() == 1, "partial sell should output one position");

    if (!partialSellResult.positionList.positions.empty()) {
        const auto& position = partialSellResult.positionList.positions.front();
        failures += expect(position.accountId == "ACC-DEMO-001", "partial sell position accountId should match input");
        failures += expect(position.portfolioId == "PORT-DEMO-001", "partial sell position portfolioId should match input");
        failures += expect(position.instrumentCode == "159509", "partial sell instrumentCode should match input");
        failures += expect(position.quantityText == "600", "partial sell remaining quantity should be 600");
        failures += expect(position.costAmountText == "600.60 CNY", "partial sell remaining cost should be 600.60 CNY");
        failures += expect(position.costPriceText == "1.001 CNY", "partial sell cost price should remain 1.001 CNY");
        failures += expect(position.currency == "CNY", "partial sell position currency should be CNY");
        failures += expect(position.dataQualityStatus == "OK", "partial sell position data quality should be OK");
    }

    failures += expect(partialSellResult.hasCashSummary, "partial sell should output cash summary");
    failures += expect(partialSellResult.cashSummary.cashBalanceText == "99478.00 CNY",
                       "partial sell cash balance should include net sell inflow");
    failures += expect(partialSellResult.cashSummary.dataQualityStatus == "OK", "partial sell cash data quality should be OK");

    failures += expect(partialSellResult.hasPortfolioPnl, "partial sell should output portfolio pnl shell");
    failures += expect(partialSellResult.portfolioPnl.realizedPnlText == "78.60 CNY",
                       "partial sell should output realized PnL");
    failures += expect(partialSellResult.portfolioPnl.unrealizedPnlText == "UNAVAILABLE",
                       "partial sell should not fabricate unrealized PnL");
    failures += expect(partialSellResult.portfolioPnl.totalAssetsText == "UNAVAILABLE",
                       "partial sell should not fabricate market value");
    failures += expect(partialSellResult.portfolioPnl.totalPnlText == "UNAVAILABLE",
                       "partial sell should not fabricate total PnL");
    failures += expect(!partialSellResult.hasBasePosition, "partial sell should not output base position");
    failures += expect(!partialSellResult.hasSniperPool, "partial sell should not output sniper pool");

    const auto sellExceedsResult =
        engine.replayReadOnly(request, {makeBuy(), makeSell("159509", "1200")}, {makeInitialCash()}, {}, {});
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Error,
                       "sell quantity greater than buy quantity should return ERROR");
    failures += expect(!sellExceedsResult.issues.empty() &&
                           sellExceedsResult.issues.front().code == AccountingIssueCode::SellExceedsPosition,
                       "sell quantity greater than buy quantity should return SELL_EXCEEDS_POSITION");
    failures += expect(sellExceedsResult.positionList.positions.empty(), "sell exceeds should not output positions");

    const auto onlySellResult = engine.replayReadOnly(request, {makeSell()}, {makeInitialCash()}, {}, {});
    failures += expect(onlySellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "only SELL fact should remain unsupported");

    const auto twoBuyOneSellResult =
        engine.replayReadOnly(request, {makeBuy(), makeBuy(), makeSell()}, {makeInitialCash()}, {}, {});
    failures += expect(twoBuyOneSellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "two BUY one SELL should remain unsupported");

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
                               makeSell("159509", "400", "480.00", "1.00", "USD")},
                              {makeInitialCash("100000.00", "USD")},
                              {},
                              {});
    failures += expect(unsupportedCurrencyResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "USD partial sell should remain unsupported");

    auto missingSellFee = makeSell();
    missingSellFee.feeText.clear();
    const auto missingSellFeeResult =
        engine.replayReadOnly(request, {makeBuy(), missingSellFee}, {makeInitialCash()}, {}, {});
    failures += expect(missingSellFeeResult.status == AccountingReplayStatus::InvalidRequest,
                       "missing sell fee should return INVALID_REQUEST");
    failures += expect(!missingSellFeeResult.issues.empty() &&
                           missingSellFeeResult.issues.front().code == AccountingIssueCode::MissingRequiredField,
                       "missing sell fee should not default to zero");

    const auto invalidMoneyResult =
        engine.replayReadOnly(request, {makeBuy(), makeSell("159509", "400", "not-money")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidMoneyResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid money text should return INVALID_REQUEST");

    const auto invalidQuantityResult =
        engine.replayReadOnly(request, {makeBuy(), makeSell("159509", "many")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidQuantityResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid quantity text should return INVALID_REQUEST");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.buySellPartialReplaySupported, "buySellPartialReplaySupported should be true");
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
