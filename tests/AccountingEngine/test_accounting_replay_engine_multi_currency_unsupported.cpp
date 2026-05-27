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

bool hasIssueCode(const etfdt::accounting::AccountingReplayResult& result, const std::string& code)
{
    return std::any_of(result.issues.begin(), result.issues.end(), [&code](const auto& issue) {
        return issue.code == code;
    });
}

bool issueBlocking(const etfdt::accounting::AccountingReplayResult& result, const std::string& code)
{
    const auto found = std::find_if(result.issues.begin(), result.issues.end(), [&code](const auto& issue) {
        return issue.code == code;
    });
    return found != result.issues.end() && found->blocking;
}

etfdt::accounting::ReplayRequestDto makeRequest()
{
    etfdt::accounting::ReplayRequestDto request;
    request.portfolioId = "PORT-DEMO-001";
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    return request;
}

etfdt::accounting::CashFactDto makeInitialCash(const std::string& currency = "CNY",
                                               const std::string& amountText = "100000.00")
{
    return etfdt::accounting::CashFactDto{
        "CASH-" + currency,
        "2026-01-01T00:00:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        etfdt::accounting::CashAction::InitialCash,
        amountText,
        currency,
        "multi-currency unsupported test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& factId,
                                        const std::string& instrumentCode,
                                        const std::string& currency,
                                        const std::string& quantityText,
                                        const std::string& amountText,
                                        const std::string& feeText = "1.00")
{
    return etfdt::accounting::TradeFactDto{
        factId,
        "2026-01-02T09:30:00Z",
        "ACC-DEMO-001",
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
        "multi-currency unsupported",
    };
}

std::vector<etfdt::accounting::TradeFactDto> makeCnyUsdBuyFacts()
{
    return {
        makeBuy("TRADE-CNY", "159509", "CNY", "1000", "1000.00"),
        makeBuy("TRADE-USD", "US-ETF", "USD", "10", "100.00"),
    };
}

etfdt::accounting::FxRateFactDto makeUsdCnyRate()
{
    return etfdt::accounting::FxRateFactDto{
        "USD",
        "CNY",
        "2026-01-02T09:30:00Z",
        "7.2000",
        "unit-test",
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
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "159509", "CNY", "1000", "1000.00")},
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(singleBuyResult.status == AccountingReplayStatus::Ok, "single buy should still pass");

    const auto partialSellResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-BUY", "159509", "CNY", "1000", "1000.00"),
                                  TradeFactDto{
                                      "TRADE-SELL",
                                      "2026-01-03T09:30:00Z",
                                      "ACC-DEMO-001",
                                      "PORT-DEMO-001",
                                      "159509",
                                      TradeAction::Sell,
                                      "400",
                                      "1.2000",
                                      "480.00",
                                      "1.00",
                                      "480.00",
                                      "CNY",
                                      "unit-test",
                                      "partial sell",
                                  },
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell should still pass");

    const auto sellExceedsResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-BUY", "159509", "CNY", "1000", "1000.00"),
                                  TradeFactDto{
                                      "TRADE-SELL",
                                      "2026-01-03T09:30:00Z",
                                      "ACC-DEMO-001",
                                      "PORT-DEMO-001",
                                      "159509",
                                      TradeAction::Sell,
                                      "1200",
                                      "1.2000",
                                      "1440.00",
                                      "1.00",
                                      "1440.00",
                                      "CNY",
                                      "unit-test",
                                      "sell exceeds",
                                  },
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Error,
                       "sell exceeds position should still pass");

    const auto missingFeeResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-MISSING-FEE", "159509", "CNY", "1000", "1000.00", "")},
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning, "missing fee should still pass");
    failures += expect(hasIssueCode(missingFeeResult, AccountingIssueCode::MissingFee),
                       "missing fee should still return MISSING_FEE");

    const auto negativeCashResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-NEGATIVE", "159509", "CNY", "1000", "1000.00")},
                              {makeInitialCash("CNY", "100.00")},
                              {},
                              {});
    failures += expect(negativeCashResult.status == AccountingReplayStatus::Error, "negative cash should still pass");
    failures += expect(hasIssueCode(negativeCashResult, AccountingIssueCode::NegativeCash),
                       "negative cash should still return NEGATIVE_CASH");

    const auto multiInstrumentResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "CNY", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "518880", "CNY", "2000", "2000.00"),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(multiInstrumentResult.status == AccountingReplayStatus::Ok,
                       "multi-instrument buy should still pass");

    const auto multiAccountResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-ACC-001", "159509", "CNY", "1000", "1000.00"),
                                  TradeFactDto{
                                      "TRADE-ACC-002",
                                      "2026-01-02T09:30:00Z",
                                      "ACC-DEMO-002",
                                      "PORT-DEMO-001",
                                      "159509",
                                      TradeAction::Buy,
                                      "2000",
                                      "1.0000",
                                      "2000.00",
                                      "1.00",
                                      "-2000.00",
                                      "CNY",
                                      "unit-test",
                                      "multi-account",
                                  },
                              },
                              {
                                  makeInitialCash("CNY", "50000.00"),
                                  CashFactDto{
                                      "CASH-ACC-DEMO-002",
                                      "2026-01-01T00:00:00Z",
                                      "ACC-DEMO-002",
                                      "PORT-DEMO-001",
                                      CashAction::InitialCash,
                                      "50000.00",
                                      "CNY",
                                      "multi-account",
                                  },
                              },
                              {},
                              {});
    failures += expect(multiAccountResult.status == AccountingReplayStatus::Ok,
                       "multi-account buy should still pass");

    const auto multiCurrencyResult =
        engine.replayReadOnly(request, makeCnyUsdBuyFacts(), {makeInitialCash()}, {}, {});
    failures += expect(multiCurrencyResult.implemented, "multi-currency unsupported should be implemented");
    failures += expect(multiCurrencyResult.replayExecuted, "multi-currency unsupported should execute replay");
    failures += expect(multiCurrencyResult.status == AccountingReplayStatus::Error,
                       "multi-currency unsupported status should be ERROR");
    failures += expect(hasIssueCode(multiCurrencyResult, AccountingIssueCode::MultiCurrencyUnsupported),
                       "multi-currency unsupported should return MULTI_CURRENCY_UNSUPPORTED");
    failures += expect(hasIssueCode(multiCurrencyResult, AccountingIssueCode::FxRateMissing),
                       "multi-currency unsupported should return FX_RATE_MISSING");
    failures += expect(issueBlocking(multiCurrencyResult, AccountingIssueCode::MultiCurrencyUnsupported),
                       "MULTI_CURRENCY_UNSUPPORTED should be blocking");
    failures += expect(issueBlocking(multiCurrencyResult, AccountingIssueCode::FxRateMissing),
                       "FX_RATE_MISSING should be blocking");
    failures += expect(multiCurrencyResult.positionList.positions.empty(),
                       "multi-currency unsupported should not output successful positions");
    failures += expect(!multiCurrencyResult.hasCashSummary,
                       "multi-currency unsupported should not output successful cashSummary");
    failures += expect(multiCurrencyResult.accountCashSummaries.empty(),
                       "multi-currency unsupported should not output account cash summaries");
    failures += expect(!multiCurrencyResult.hasPortfolioPnl,
                       "multi-currency unsupported should not output successful portfolioPnl");
    failures += expect(!multiCurrencyResult.hasBasePosition, "multi-currency unsupported should not output basePosition");
    failures += expect(!multiCurrencyResult.hasSniperPool, "multi-currency unsupported should not output sniperPool");
    failures += expect(multiCurrencyResult.portfolioPnl.totalAssetsText.empty(),
                       "multi-currency unsupported should not fabricate totalAssetsText");
    failures += expect(multiCurrencyResult.positionList.positions.empty(),
                       "multi-currency unsupported should not fabricate marketValueText");
    failures += expect(multiCurrencyResult.portfolioPnl.unrealizedPnlText.empty(),
                       "multi-currency unsupported should not fabricate unrealizedPnlText");

    const auto withFxRateResult =
        engine.replayReadOnly(request, makeCnyUsdBuyFacts(), {makeInitialCash()}, {}, {makeUsdCnyRate()});
    failures += expect(withFxRateResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "multi-currency with FX rate should still not do FX conversion");
    failures += expect(!withFxRateResult.implemented, "multi-currency with FX rate should not be implemented");
    failures += expect(!withFxRateResult.replayExecuted, "multi-currency with FX rate should not execute replay");

    const auto singleCurrencyResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "159509", "CNY", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "518880", "CNY", "2000", "2000.00"),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(singleCurrencyResult.status != AccountingReplayStatus::Error ||
                           !hasIssueCode(singleCurrencyResult, AccountingIssueCode::MultiCurrencyUnsupported),
                       "single CNY facts should not trigger multi-currency unsupported");

    const auto singleUsdResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-USD-1", "US-ETF", "USD", "10", "100.00"),
                                  makeBuy("TRADE-USD-2", "US-ETF-2", "USD", "20", "200.00"),
                              },
                              {makeInitialCash("USD", "1000.00")},
                              {},
                              {});
    failures += expect(singleUsdResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "single USD currency should remain unsupported but not multi-currency");

    const auto missingFeeMultiCurrency =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-CNY", "159509", "CNY", "1000", "1000.00"),
                                  makeBuy("TRADE-USD", "US-ETF", "USD", "10", "100.00", ""),
                              },
                              {makeInitialCash()},
                              {},
                              {});
    failures += expect(missingFeeMultiCurrency.status == AccountingReplayStatus::Warning,
                       "missing fee should keep priority before multi-currency error");
    failures += expect(hasIssueCode(missingFeeMultiCurrency, AccountingIssueCode::MissingFee),
                       "missing fee priority should return MISSING_FEE");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.multiCurrencyUnsupportedDetectionSupported,
                       "multiCurrencyUnsupportedDetectionSupported should be true");
    failures += expect(!capabilities.writeEnabled, "capabilities.writeEnabled should remain false");
    failures += expect(!capabilities.snapshotWriteEnabled, "capabilities.snapshotWriteEnabled should remain false");
    failures += expect(!capabilities.tradeLogWriteEnabled, "capabilities.tradeLogWriteEnabled should remain false");
    failures += expect(!capabilities.supportsFxRate, "supportsFxRate should remain false");
    failures += expect(!capabilities.supportsMultiCurrency, "supportsMultiCurrency should remain false");

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
