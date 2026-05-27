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
        "single buy test",
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
        "single buy test",
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
    failures += expect(emptyResult.status == AccountingReplayStatus::Ok, "empty ledger test should still pass");

    const auto singleBuyResult = engine.replayReadOnly(request, {makeBuy()}, {makeInitialCash()}, {}, {});
    failures += expect(singleBuyResult.implemented, "single buy should be implemented");
    failures += expect(singleBuyResult.replayExecuted, "single buy should execute replay");
    failures += expect(singleBuyResult.status == AccountingReplayStatus::Ok, "single buy status should be OK");
    failures += expect(singleBuyResult.issues.empty(), "single buy should not contain issues");
    failures += expect(singleBuyResult.positionList.positions.size() == 1, "single buy should output one position");

    if (!singleBuyResult.positionList.positions.empty()) {
        const auto& position = singleBuyResult.positionList.positions.front();
        failures += expect(position.accountId == "ACC-DEMO-001", "single buy position accountId should match input");
        failures += expect(position.portfolioId == "PORT-DEMO-001", "single buy position portfolioId should match input");
        failures += expect(position.instrumentCode == "159509", "single buy instrumentCode should match input");
        failures += expect(position.quantityText == "1000", "single buy quantityText should be 1000");
        failures +=
            expect(position.costAmountText == "1001.00 CNY", "single buy costAmountText should include amount plus fee");
        failures += expect(position.costPriceText == "1.001 CNY", "single buy costPriceText should divide cost by quantity");
        failures += expect(position.currency == "CNY", "single buy position currency should be CNY");
        failures += expect(position.dataQualityStatus == "OK", "single buy position data quality should be OK");
    }

    failures += expect(singleBuyResult.hasCashSummary, "single buy should output cash summary");
    failures += expect(singleBuyResult.cashSummary.cashBalanceText == "98999.00 CNY",
                       "single buy cash balance should subtract amount and fee");
    failures += expect(singleBuyResult.cashSummary.dataQualityStatus == "OK", "single buy cash data quality should be OK");

    failures += expect(singleBuyResult.hasPortfolioPnl, "single buy should output portfolio pnl shell");
    failures += expect(singleBuyResult.portfolioPnl.totalAssetsText == "UNAVAILABLE",
                       "single buy should not fabricate market value");
    failures += expect(singleBuyResult.portfolioPnl.totalPnlText == "UNAVAILABLE",
                       "single buy should not fabricate unrealized pnl");
    failures += expect(singleBuyResult.portfolioPnl.dataQualityStatus == "UNAVAILABLE",
                       "single buy portfolio pnl should remain unavailable");
    failures += expect(!singleBuyResult.hasBasePosition, "single buy should not output base position");
    failures += expect(!singleBuyResult.hasSniperPool, "single buy should not output sniper pool");

    auto missingFee = makeBuy();
    missingFee.feeText.clear();
    const auto missingFeeResult = engine.replayReadOnly(request, {missingFee}, {makeInitialCash()}, {}, {});
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning,
                       "missing fee should return WARNING");
    failures += expect(missingFeeResult.issues.front().code == AccountingIssueCode::MissingFee,
                       "missing fee should not default to zero");

    auto sellFact = makeBuy();
    sellFact.action = TradeAction::Sell;
    const auto sellResult = engine.replayReadOnly(request, {sellFact}, {makeInitialCash()}, {}, {});
    failures += expect(sellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "SELL should remain unsupported");

    const auto twoBuysResult = engine.replayReadOnly(request, {makeBuy(), makeBuy()}, {makeInitialCash()}, {}, {});
    failures += expect(twoBuysResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "two BUY facts should remain unsupported");

    const auto multiInstrumentResult =
        engine.replayReadOnly(request, {makeBuy("159509"), makeBuy("518880")}, {makeInitialCash()}, {}, {});
    failures += expect(multiInstrumentResult.status == AccountingReplayStatus::Ok,
                       "multi-instrument BUY facts should be handled by the dedicated boundary");
    failures += expect(multiInstrumentResult.positionList.positions.size() == 2,
                       "multi-instrument BUY should output separate positions");

    const auto invalidMoneyResult = engine.replayReadOnly(request, {makeBuy("159509", "1000", "not-money")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidMoneyResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid money text should return INVALID_REQUEST");

    const auto invalidQuantityResult = engine.replayReadOnly(request, {makeBuy("159509", "many")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidQuantityResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid quantity text should return INVALID_REQUEST");

    const auto unsupportedCurrencyResult = engine.replayReadOnly(request, {makeBuy("159509", "1000", "1000.00", "1.00", "USD")},
                                                                 {makeInitialCash("100000.00", "USD")},
                                                                 {},
                                                                 {});
    failures += expect(unsupportedCurrencyResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "USD single buy should remain unsupported");

    const auto negativeCashResult = engine.replayReadOnly(request, {makeBuy()}, {makeInitialCash("100.00")}, {}, {});
    failures += expect(negativeCashResult.implemented, "negative cash should be implemented");
    failures += expect(negativeCashResult.replayExecuted, "negative cash should execute replay");
    failures += expect(negativeCashResult.status == AccountingReplayStatus::Error,
                       "negative cash should not succeed as overdraft");
    failures += expect(!negativeCashResult.issues.empty() &&
                           negativeCashResult.issues.front().code == AccountingIssueCode::NegativeCash,
                       "negative cash should return NEGATIVE_CASH");
    failures += expect(negativeCashResult.positionList.positions.empty(), "negative cash should not output positions");
    failures += expect(!negativeCashResult.hasCashSummary, "negative cash should not output cash summary");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.singleBuyReplaySupported, "capabilities.singleBuyReplaySupported should be true");
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
