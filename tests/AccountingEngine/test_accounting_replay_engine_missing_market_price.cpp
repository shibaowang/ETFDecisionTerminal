#include "AccountingEngine/AccountingEngine.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

int expect(bool condition, const std::string& message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "FAILED: " << message << '\n';
    return 1;
}

bool contains(const std::vector<std::string>& values, const std::string& expected)
{
    return std::find(values.begin(), values.end(), expected) != values.end();
}

bool hasIssueCode(const etfdt::accounting::AccountingReplayResult& result, const std::string& code)
{
    return std::any_of(result.issues.begin(), result.issues.end(), [&code](const auto& issue) {
        return issue.code == code;
    });
}

bool issueBlocking(const etfdt::accounting::AccountingReplayResult& result, const std::string& code)
{
    const auto issue = std::find_if(result.issues.begin(), result.issues.end(), [&code](const auto& item) {
        return item.code == code;
    });
    return issue != result.issues.end() && issue->blocking;
}

std::string issueLevel(const etfdt::accounting::AccountingReplayResult& result, const std::string& code)
{
    const auto issue = std::find_if(result.issues.begin(), result.issues.end(), [&code](const auto& item) {
        return item.code == code;
    });
    return issue == result.issues.end() ? std::string{} : issue->level;
}

etfdt::accounting::ReplayRequestDto makeRequest(bool requestPnl = false)
{
    etfdt::accounting::ReplayRequestDto request;
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PORT-DEMO-001";
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    if (requestPnl) {
        request.requestedOutputs.push_back(etfdt::accounting::RequestedReplayOutput::Pnl);
    }
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
        "missing market price test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& instrumentCode = "159509",
                                        const std::string& quantityText = "1000",
                                        const std::string& amountText = "1000.00",
                                        const std::string& feeText = "1.00",
                                        const std::string& currency = "CNY")
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-001",
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
        "missing market price test",
    };
}

etfdt::accounting::MarketPriceFactDto makeMarketPrice(const std::string& instrumentCode = "159509")
{
    return etfdt::accounting::MarketPriceFactDto{
        instrumentCode,
        "2026-01-02T15:00:00Z",
        "1.20",
        "CNY",
        "unit-test",
    };
}

etfdt::accounting::TradeFactDto makeUsdBuy()
{
    auto tradeFact = makeBuy("US-ETF", "10", "100.00", "1.00", "USD");
    tradeFact.factId = "TRADE-USD";
    return tradeFact;
}

} // namespace

int main()
{
    using namespace etfdt::accounting;

    int failures = 0;
    AccountingReplayEngine engine;

    const auto normalRequest = makeRequest();
    const auto valuationRequest = makeRequest(true);

    failures += expect(engine.replayReadOnly(normalRequest, {}, {}, {}, {}).status == AccountingReplayStatus::Ok,
                       "empty ledger should still pass");

    const auto normalSingleBuy = engine.replayReadOnly(normalRequest, {makeBuy()}, {makeInitialCash()}, {}, {});
    failures += expect(normalSingleBuy.status == AccountingReplayStatus::Ok,
                       "ordinary single BUY without valuation request should remain OK");
    failures += expect(!hasIssueCode(normalSingleBuy, AccountingIssueCode::MarketPriceMissing),
                       "ordinary single BUY should not emit MARKET_PRICE_MISSING");

    const auto missingMarketPrice =
        engine.replayReadOnly(valuationRequest, {makeBuy()}, {makeInitialCash()}, {}, {});
    failures += expect(missingMarketPrice.implemented, "missing market price should be implemented");
    failures += expect(missingMarketPrice.replayExecuted, "missing market price should execute replay");
    failures += expect(missingMarketPrice.status == AccountingReplayStatus::Warning,
                       "missing market price should return WARNING");
    failures += expect(hasIssueCode(missingMarketPrice, AccountingIssueCode::MarketPriceMissing),
                       "missing market price should emit MARKET_PRICE_MISSING");
    failures += expect(issueLevel(missingMarketPrice, AccountingIssueCode::MarketPriceMissing) == AccountingIssueLevel::Warning,
                       "MARKET_PRICE_MISSING issue should be WARNING");
    failures += expect(!issueBlocking(missingMarketPrice, AccountingIssueCode::MarketPriceMissing),
                       "MARKET_PRICE_MISSING issue should be non-blocking");
    failures += expect(missingMarketPrice.positionList.positions.size() == 1,
                       "missing market price can output position quantity and cost");
    if (!missingMarketPrice.positionList.positions.empty()) {
        const auto& position = missingMarketPrice.positionList.positions.front();
        failures += expect(position.quantityText == "1000", "missing market price position quantity should be 1000");
        failures += expect(position.costAmountText == "1001.00 CNY",
                           "missing market price position cost should include amount plus fee");
        failures += expect(position.marketValueText == "UNAVAILABLE",
                           "missing market price must not fabricate marketValueText");
        failures += expect(position.unrealizedPnlText == "UNAVAILABLE",
                           "missing market price must not fabricate unrealizedPnlText");
    }
    failures += expect(missingMarketPrice.hasCashSummary, "missing market price can output cash summary");
    failures += expect(missingMarketPrice.cashSummary.cashBalanceText == "98999.00 CNY",
                       "missing market price cash balance should be deterministic");
    failures += expect(missingMarketPrice.hasPortfolioPnl, "missing market price should output unavailable pnl shell");
    failures += expect(missingMarketPrice.portfolioPnl.totalAssetsText == "UNAVAILABLE",
                       "missing market price must not fabricate totalAssetsText");
    failures += expect(missingMarketPrice.portfolioPnl.unrealizedPnlText == "UNAVAILABLE",
                       "missing market price must not fabricate portfolio unrealizedPnlText");
    failures += expect(!missingMarketPrice.hasBasePosition, "missing market price should not output base position");
    failures += expect(!missingMarketPrice.hasSniperPool, "missing market price should not output sniper pool");

    const auto withMarketPrice =
        engine.replayReadOnly(valuationRequest, {makeBuy()}, {makeInitialCash()}, {makeMarketPrice()}, {});
    failures += expect(withMarketPrice.status == AccountingReplayStatus::UnsupportedScenario,
                       "valid marketPriceFact should not unlock valuation success in this task");

    auto missingFee = makeBuy();
    missingFee.feeText.clear();
    const auto missingFeeResult = engine.replayReadOnly(valuationRequest, {missingFee}, {makeInitialCash()}, {}, {});
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning,
                       "missing fee priority should remain WARNING");
    failures += expect(hasIssueCode(missingFeeResult, AccountingIssueCode::MissingFee),
                       "missing fee priority should return MISSING_FEE");
    failures += expect(!hasIssueCode(missingFeeResult, AccountingIssueCode::MarketPriceMissing),
                       "missing fee should not be overwritten by MARKET_PRICE_MISSING");

    const auto negativeCash =
        engine.replayReadOnly(valuationRequest, {makeBuy()}, {makeInitialCash("100.00")}, {}, {});
    failures += expect(negativeCash.status == AccountingReplayStatus::Error,
                       "negative cash priority should remain ERROR");
    failures += expect(hasIssueCode(negativeCash, AccountingIssueCode::NegativeCash),
                       "negative cash priority should return NEGATIVE_CASH");
    failures += expect(!hasIssueCode(negativeCash, AccountingIssueCode::MarketPriceMissing),
                       "negative cash should not be overwritten by MARKET_PRICE_MISSING");

    const auto multiCurrency = engine.replayReadOnly(
        valuationRequest,
        {makeBuy(), makeUsdBuy()},
        {makeInitialCash()},
        {},
        {});
    failures += expect(multiCurrency.status == AccountingReplayStatus::Error,
                       "multi-currency priority should remain ERROR");
    failures += expect(hasIssueCode(multiCurrency, AccountingIssueCode::MultiCurrencyUnsupported),
                       "multi-currency priority should return MULTI_CURRENCY_UNSUPPORTED");
    failures += expect(hasIssueCode(multiCurrency, AccountingIssueCode::FxRateMissing),
                       "multi-currency priority should return FX_RATE_MISSING");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.missingMarketPriceDetectionSupported,
                       "missingMarketPriceDetectionSupported should be true");
    failures += expect(!capabilities.writeEnabled, "capabilities.writeEnabled should remain false");
    failures += expect(!capabilities.snapshotWriteEnabled, "capabilities.snapshotWriteEnabled should remain false");
    failures += expect(!capabilities.tradeLogWriteEnabled, "capabilities.tradeLogWriteEnabled should remain false");
    failures += expect(!capabilities.supportsMarketPrice, "supportsMarketPrice should remain false");

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
