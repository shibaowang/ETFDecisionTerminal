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
        "negative cash test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& amountText = "1000.00",
                                        const std::string& feeText = "1.00",
                                        const std::string& quantityText = "1000",
                                        const std::string& currency = "CNY")
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-BUY-001",
        "2026-01-02T09:30:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        "159509",
        etfdt::accounting::TradeAction::Buy,
        quantityText,
        "1.0000",
        amountText,
        feeText,
        "-1001.00",
        currency,
        "unit-test",
        "negative cash buy",
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
        "negative cash sell",
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
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell should still pass");

    const auto sellExceedsResult =
        engine.replayReadOnly(request, {makeBuy("1000.00", "1.00", "1000"), makeSell()}, {makeInitialCash()}, {}, {});
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Ok,
                       "supported partial sell should not become sell exceeds");

    auto sellExceedsTrade = makeSell();
    sellExceedsTrade.quantityText = "1200";
    const auto actualSellExceedsResult =
        engine.replayReadOnly(request, {makeBuy(), sellExceedsTrade}, {makeInitialCash()}, {}, {});
    failures += expect(actualSellExceedsResult.status == AccountingReplayStatus::Error,
                       "sell exceeds position should still return ERROR");
    failures += expect(hasIssueCode(actualSellExceedsResult, AccountingIssueCode::SellExceedsPosition),
                       "sell exceeds position should still return SELL_EXCEEDS_POSITION");

    const auto missingFeeResult = engine.replayReadOnly(request, {makeBuy("1000.00", "")}, {makeInitialCash("100.00")}, {}, {});
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning,
                       "missing fee should take precedence over negative cash");
    failures += expect(hasIssueCode(missingFeeResult, AccountingIssueCode::MissingFee),
                       "missing fee should keep MISSING_FEE issue");
    failures += expect(!hasIssueCode(missingFeeResult, AccountingIssueCode::NegativeCash),
                       "missing fee should not be overwritten by NEGATIVE_CASH");

    const auto negativeCashResult =
        engine.replayReadOnly(request, {makeBuy("1000.00", "1.00")}, {makeInitialCash("100.00")}, {}, {});
    failures += expect(negativeCashResult.implemented, "negative cash should be implemented");
    failures += expect(negativeCashResult.replayExecuted, "negative cash should execute replay");
    failures += expect(negativeCashResult.status == AccountingReplayStatus::Error, "negative cash status should be ERROR");
    failures += expect(negativeCashResult.message == "Single buy cash requirement exceeds available cash.",
                       "negative cash message should be stable");
    failures += expect(negativeCashResult.issues.size() == 1, "negative cash should have one issue");
    if (!negativeCashResult.issues.empty()) {
        const auto& issue = negativeCashResult.issues.front();
        failures += expect(issue.code == AccountingIssueCode::NegativeCash,
                           "negative cash issue code should be NEGATIVE_CASH");
        failures += expect(issue.level == AccountingIssueLevel::Error, "negative cash issue level should be ERROR");
        failures += expect(issue.blocking, "negative cash issue should be blocking");
    }
    failures += expect(negativeCashResult.positionList.positions.empty(),
                       "negative cash should not output success positions");
    failures += expect(!negativeCashResult.hasCashSummary, "negative cash should not output cash summary");
    failures += expect(!negativeCashResult.hasPortfolioPnl, "negative cash should not output portfolio PnL");
    failures += expect(!negativeCashResult.hasBasePosition, "negative cash should not output base position");
    failures += expect(!negativeCashResult.hasSniperPool, "negative cash should not output sniper pool");

    const auto exactCashResult =
        engine.replayReadOnly(request, {makeBuy("999.00", "1.00")}, {makeInitialCash("1000.00")}, {}, {});
    failures += expect(exactCashResult.status == AccountingReplayStatus::Ok,
                       "amount plus fee equal to initial cash should not be NEGATIVE_CASH");
    failures += expect(exactCashResult.hasCashSummary, "exact cash single buy should output cash summary");
    failures += expect(exactCashResult.cashSummary.cashBalanceText == "0.00 CNY",
                       "exact cash single buy should leave zero cash");
    failures += expect(!hasIssueCode(exactCashResult, AccountingIssueCode::NegativeCash),
                       "exact cash single buy should not have NEGATIVE_CASH");

    const auto explicitZeroFeeResult =
        engine.replayReadOnly(request, {makeBuy("1000.00", "0.00")}, {makeInitialCash("1000.00")}, {}, {});
    failures += expect(explicitZeroFeeResult.status == AccountingReplayStatus::Ok,
                       "explicit zero fee should not trigger MISSING_FEE or NEGATIVE_CASH");
    failures += expect(!hasIssueCode(explicitZeroFeeResult, AccountingIssueCode::MissingFee),
                       "explicit zero fee should not have MISSING_FEE");

    const auto invalidAmountResult =
        engine.replayReadOnly(request, {makeBuy("not-money", "1.00")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidAmountResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid amount should return INVALID_REQUEST");

    const auto invalidFeeResult =
        engine.replayReadOnly(request, {makeBuy("1000.00", "free")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidFeeResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid fee should return INVALID_REQUEST");

    const auto unsupportedCurrencyResult =
        engine.replayReadOnly(request, {makeBuy("1000.00", "1.00", "1000", "USD")},
                              {makeInitialCash("100.00", "USD")},
                              {},
                              {});
    failures += expect(unsupportedCurrencyResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "USD negative cash should remain unsupported");

    const auto sellOnlyResult = engine.replayReadOnly(request, {makeSell()}, {makeInitialCash("100.00")}, {}, {});
    failures += expect(sellOnlyResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "SELL-only negative cash is outside this task");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.negativeCashDetectionSupported, "negativeCashDetectionSupported should be true");
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
