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

const etfdt::accounting::PositionSummaryDto* findPosition(
    const etfdt::accounting::AccountingReplayResult& result,
    const std::string& instrumentCode)
{
    const auto found = std::find_if(
        result.positionList.positions.begin(),
        result.positionList.positions.end(),
        [&instrumentCode](const auto& position) { return position.instrumentCode == instrumentCode; });
    return found == result.positionList.positions.end() ? nullptr : &(*found);
}

bool hasIssueCode(const etfdt::accounting::AccountingReplayResult& result, const std::string& code)
{
    return std::any_of(result.issues.begin(), result.issues.end(), [&code](const auto& issue) {
        return issue.code == code;
    });
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
        "multi-instrument buy test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& factId,
                                        const std::string& instrumentCode,
                                        const std::string& quantityText,
                                        const std::string& amountText,
                                        const std::string& feeText = "1.00",
                                        const std::string& accountId = "ACC-DEMO-001",
                                        const std::string& currency = "CNY")
{
    return etfdt::accounting::TradeFactDto{
        factId,
        "2026-01-02T09:30:00Z",
        accountId,
        "PORT-DEMO-001",
        instrumentCode,
        etfdt::accounting::TradeAction::Buy,
        quantityText,
        "1.0000",
        amountText,
        feeText,
        "-" + amountText,
        currency,
        "unit-test",
        "multi-instrument buy",
    };
}

etfdt::accounting::TradeFactDto makeSell()
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-SELL-001",
        "2026-01-03T09:30:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        "159509",
        etfdt::accounting::TradeAction::Sell,
        "400",
        "1.2000",
        "480.00",
        "1.00",
        "480.00",
        "CNY",
        "unit-test",
        "multi-instrument sell",
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

    const auto singleBuyResult =
        engine.replayReadOnly(request, {makeBuy("TRADE-001", "159509", "1000", "1000.00")}, {makeInitialCash()}, {}, {});
    failures += expect(singleBuyResult.status == AccountingReplayStatus::Ok, "single buy should still pass");

    const auto partialSellResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "159509", "1000", "1000.00"), makeSell()},
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell should still pass");

    auto sellExceedsTrade = makeSell();
    sellExceedsTrade.quantityText = "1200";
    const auto sellExceedsResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "159509", "1000", "1000.00"), sellExceedsTrade},
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Error,
                       "sell exceeds position should still pass");

    const auto missingFeeResult = engine.replayReadOnly(
        request,
        {makeBuy("TRADE-001", "159509", "1000", "1000.00", "")},
        {makeInitialCash()},
        {},
        {});
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning, "missing fee should still pass");
    failures += expect(hasIssueCode(missingFeeResult, AccountingIssueCode::MissingFee),
                       "missing fee should still return MISSING_FEE");

    const auto negativeCashResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "159509", "1000", "1000.00")},
                              {makeInitialCash("100.00")},
                              {},
                              {});
    failures += expect(negativeCashResult.status == AccountingReplayStatus::Error, "negative cash should still pass");
    failures += expect(hasIssueCode(negativeCashResult, AccountingIssueCode::NegativeCash),
                       "negative cash should still return NEGATIVE_CASH");

    const auto multiInstrumentResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "518880", "2000", "2000.00"),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(multiInstrumentResult.implemented, "multi-instrument buy should be implemented");
    failures += expect(multiInstrumentResult.replayExecuted, "multi-instrument buy should execute replay");
    failures += expect(multiInstrumentResult.status == AccountingReplayStatus::Ok, "multi-instrument buy status should be OK");
    failures += expect(multiInstrumentResult.positionList.positions.size() == 2,
                       "multi-instrument buy should output two positions");

    const auto* position159509 = findPosition(multiInstrumentResult, "159509");
    const auto* position518880 = findPosition(multiInstrumentResult, "518880");
    failures += expect(position159509 != nullptr, "multi-instrument buy should contain 159509");
    failures += expect(position518880 != nullptr, "multi-instrument buy should contain 518880");
    if (position159509 != nullptr) {
        failures += expect(position159509->quantityText == "1000", "159509 quantityText should be independent");
        failures += expect(position159509->costAmountText == "1001.00 CNY", "159509 costAmountText should aggregate only 159509");
        failures += expect(position159509->costPriceText == "1.001 CNY", "159509 costPriceText should be stable");
        failures += expect(position159509->marketValueText == "UNAVAILABLE", "159509 marketValue should be unavailable");
        failures += expect(position159509->unrealizedPnlText == "UNAVAILABLE", "159509 unrealized PnL should be unavailable");
    }
    if (position518880 != nullptr) {
        failures += expect(position518880->quantityText == "2000", "518880 quantityText should be independent");
        failures += expect(position518880->costAmountText == "2001.00 CNY", "518880 costAmountText should aggregate only 518880");
        failures += expect(position518880->costPriceText == "1.001 CNY", "518880 costPriceText should be stable");
        failures += expect(position518880->marketValueText == "UNAVAILABLE", "518880 marketValue should be unavailable");
        failures += expect(position518880->unrealizedPnlText == "UNAVAILABLE", "518880 unrealized PnL should be unavailable");
    }
    failures += expect(multiInstrumentResult.hasCashSummary, "multi-instrument buy should output cash summary");
    failures += expect(multiInstrumentResult.cashSummary.cashBalanceText == "96998.00 CNY",
                       "multi-instrument buy cash should deduct all BUY amount plus fee");
    failures += expect(multiInstrumentResult.hasPortfolioPnl, "multi-instrument buy should output portfolio pnl shell");
    failures += expect(multiInstrumentResult.portfolioPnl.totalAssetsText == "UNAVAILABLE",
                       "multi-instrument buy should not fabricate market value");
    failures += expect(multiInstrumentResult.portfolioPnl.unrealizedPnlText == "UNAVAILABLE",
                       "multi-instrument buy should not fabricate unrealized pnl");
    failures += expect(!multiInstrumentResult.hasBasePosition, "multi-instrument buy should not output base position");
    failures += expect(!multiInstrumentResult.hasSniperPool, "multi-instrument buy should not output sniper pool");

    const auto mixedWithSellResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "518880", "2000", "2000.00"),
                                  makeSell(),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(mixedWithSellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "multi-instrument with SELL should remain unsupported");

    const auto differentAccountResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "1000", "1000.00", "1.00", "ACC-DEMO-001"),
                                  makeBuy("TRADE-518880", "518880", "2000", "2000.00", "1.00", "ACC-DEMO-002"),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(differentAccountResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "multi-instrument with different account should remain unsupported");

    const auto usdResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "1000", "1000.00", "1.00", "ACC-DEMO-001", "CNY"),
                                  makeBuy("TRADE-USD", "QQQ", "10", "1000.00", "1.00", "ACC-DEMO-001", "USD"),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(usdResult.status == AccountingReplayStatus::Error,
                       "multi-instrument with USD should be rejected by multi-currency handling");
    failures += expect(hasIssueCode(usdResult, AccountingIssueCode::MultiCurrencyUnsupported),
                       "multi-instrument with USD should return MULTI_CURRENCY_UNSUPPORTED");

    const auto multiMissingFeeResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "518880", "2000", "2000.00", ""),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(multiMissingFeeResult.status == AccountingReplayStatus::Warning,
                       "multi-instrument missing fee should return MISSING_FEE warning");
    failures += expect(hasIssueCode(multiMissingFeeResult, AccountingIssueCode::MissingFee),
                       "multi-instrument missing fee should not default to zero");

    const auto multiNegativeCashResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "518880", "2000", "2000.00"),
                              },
                              {makeInitialCash("100.00")},
                              {},
                              {});
    failures += expect(multiNegativeCashResult.status == AccountingReplayStatus::Error,
                       "multi-instrument negative cash should return ERROR");
    failures += expect(hasIssueCode(multiNegativeCashResult, AccountingIssueCode::NegativeCash),
                       "multi-instrument negative cash should return NEGATIVE_CASH");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.multiInstrumentBuyReplaySupported,
                       "multiInstrumentBuyReplaySupported should be true");
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
