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

etfdt::accounting::TradeFactDto makeTradeFact(const std::string& action)
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-001",
        "2026-01-02T09:30:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        "159509",
        action,
        "100",
        "1.2500",
        "125.00",
        "0.10",
        "-125.10",
        "CNY",
        "unit-test",
        "parser boundary test",
    };
}

etfdt::accounting::CashFactDto makeCashFact(const std::string& action)
{
    return etfdt::accounting::CashFactDto{
        "CASH-001",
        "2026-01-01T00:00:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        action,
        "1000.00",
        "CNY",
        "parser boundary test",
    };
}

} // namespace

int main()
{
    using namespace etfdt::accounting;

    int failures = 0;

    const auto emptyRequest = validateReplayRequest(ReplayRequestDto{});
    failures += expect(emptyRequest.valid, "empty replay request should be valid at parser boundary");

    ReplayRequestDto requestedOutputs;
    requestedOutputs.requestedOutputs = {
        RequestedReplayOutput::Positions,
        RequestedReplayOutput::Cash,
        RequestedReplayOutput::Pnl,
        RequestedReplayOutput::BasePosition,
        RequestedReplayOutput::SniperPool,
        RequestedReplayOutput::Issues,
    };
    failures += expect(validateReplayRequest(requestedOutputs).valid, "supported requested outputs should be valid");

    ReplayRequestDto invalidOutput = requestedOutputs;
    invalidOutput.requestedOutputs.push_back("orders");
    failures += expect(validateReplayRequest(invalidOutput).hasIssueCode(AccountingIssueCode::InvalidEnum),
                       "unsupported requested output should return INVALID_ENUM");

    ReplayRequestDto invalidTimeRange;
    invalidTimeRange.sourceFromTime = "2026-02-01T00:00:00Z";
    invalidTimeRange.sourceToTime = "2026-01-01T00:00:00Z";
    failures += expect(validateReplayRequest(invalidTimeRange).hasIssueCode(AccountingIssueCode::InvalidTimeRange),
                       "sourceFromTime after sourceToTime should return INVALID_TIME_RANGE");

    failures += expect(validateTradeFact(makeTradeFact(TradeAction::Buy)).valid, "complete BUY trade fact should be valid");
    failures += expect(validateTradeFact(makeTradeFact(TradeAction::Sell)).valid, "complete SELL trade fact should be valid");

    auto missingFactId = makeTradeFact(TradeAction::Buy);
    missingFactId.factId.clear();
    failures += expect(validateTradeFact(missingFactId).hasIssueCode(AccountingIssueCode::MissingRequiredField),
                       "missing trade factId should return MISSING_REQUIRED_FIELD");

    auto missingAccountId = makeTradeFact(TradeAction::Buy);
    missingAccountId.accountId.clear();
    failures += expect(validateTradeFact(missingAccountId).hasIssueCode(AccountingIssueCode::MissingRequiredField),
                       "missing trade accountId should return MISSING_REQUIRED_FIELD");

    auto unsupportedAction = makeTradeFact("DIVIDEND");
    failures += expect(validateTradeFact(unsupportedAction).hasIssueCode(AccountingIssueCode::UnsupportedAction),
                       "unsupported trade action should return UNSUPPORTED_ACTION");

    auto invalidQuantity = makeTradeFact(TradeAction::Buy);
    invalidQuantity.quantityText = "one hundred";
    failures += expect(validateTradeFact(invalidQuantity).hasIssueCode(AccountingIssueCode::InvalidQuantityText),
                       "invalid quantity text should return INVALID_QUANTITY_TEXT");

    auto invalidAmount = makeTradeFact(TradeAction::Buy);
    invalidAmount.amountText = "CNY 100";
    failures += expect(validateTradeFact(invalidAmount).hasIssueCode(AccountingIssueCode::InvalidMoneyText),
                       "invalid trade amount text should return INVALID_MONEY_TEXT");

    auto invalidFee = makeTradeFact(TradeAction::Buy);
    invalidFee.feeText = "free";
    failures += expect(validateTradeFact(invalidFee).hasIssueCode(AccountingIssueCode::InvalidMoneyText),
                       "invalid trade fee text should return INVALID_MONEY_TEXT");

    auto invalidCurrency = makeTradeFact(TradeAction::Buy);
    invalidCurrency.currency = "CN";
    failures += expect(validateTradeFact(invalidCurrency).hasIssueCode(AccountingIssueCode::UnsupportedCurrency),
                       "invalid trade currency should return UNSUPPORTED_CURRENCY");

    failures += expect(validateCashFact(makeCashFact(CashAction::InitialCash)).valid,
                       "INITIAL_CASH cash fact should be valid");
    failures += expect(validateCashFact(makeCashFact("UNKNOWN")).hasIssueCode(AccountingIssueCode::UnsupportedAction),
                       "unsupported cash action should return UNSUPPORTED_ACTION");

    MarketPriceFactDto priceFact{
        "159509",
        "2026-01-02T15:00:00Z",
        "1.3000",
        "CNY",
        "unit-test",
    };
    failures += expect(validateMarketPriceFact(priceFact).valid, "complete market price fact should be valid");
    priceFact.priceText = "missing";
    failures += expect(validateMarketPriceFact(priceFact).hasIssueCode(AccountingIssueCode::InvalidMoneyText),
                       "invalid price text should return INVALID_MONEY_TEXT");

    FxRateFactDto fxRate{
        "USD",
        "CNY",
        "2026-01-02T15:00:00Z",
        "7.1000",
        "unit-test",
    };
    failures += expect(validateFxRateFact(fxRate).valid, "USD/CNY FX rate fact should be valid");
    fxRate.quoteCurrency = "USD";
    failures += expect(validateFxRateFact(fxRate).hasIssueCode(AccountingIssueCode::InvalidEnum),
                       "same FX base and quote currency should return INVALID_ENUM");

    failures += expect(parseTradeFactBoundary(makeTradeFact(TradeAction::Buy)).success,
                       "parseTradeFactBoundary should succeed for a valid BUY");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "AccountingEngineCapabilities.replayImplemented should remain false");
    failures += expect(!capabilities.writeEnabled, "AccountingEngineCapabilities.writeEnabled should remain false");

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
