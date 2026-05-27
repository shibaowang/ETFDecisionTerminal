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

const etfdt::accounting::PositionSummaryDto* findPositionByAccount(
    const etfdt::accounting::AccountingReplayResult& result,
    const std::string& accountId)
{
    const auto found = std::find_if(
        result.positionList.positions.begin(),
        result.positionList.positions.end(),
        [&accountId](const auto& position) { return position.accountId == accountId; });
    return found == result.positionList.positions.end() ? nullptr : &(*found);
}

const etfdt::accounting::CashSummaryDto* findCashByAccount(
    const etfdt::accounting::AccountingReplayResult& result,
    const std::string& accountId)
{
    const auto found = std::find_if(
        result.accountCashSummaries.begin(),
        result.accountCashSummaries.end(),
        [&accountId](const auto& cashSummary) { return cashSummary.accountId == accountId; });
    return found == result.accountCashSummaries.end() ? nullptr : &(*found);
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

etfdt::accounting::CashFactDto makeInitialCash(const std::string& accountId,
                                               const std::string& amountText = "50000.00",
                                               const std::string& portfolioId = "PORT-DEMO-001",
                                               const std::string& currency = "CNY")
{
    return etfdt::accounting::CashFactDto{
        "CASH-" + accountId,
        "2026-01-01T00:00:00Z",
        accountId,
        portfolioId,
        etfdt::accounting::CashAction::InitialCash,
        amountText,
        currency,
        "multi-account buy test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& factId,
                                        const std::string& accountId,
                                        const std::string& quantityText,
                                        const std::string& amountText,
                                        const std::string& feeText = "1.00",
                                        const std::string& portfolioId = "PORT-DEMO-001",
                                        const std::string& currency = "CNY",
                                        const std::string& instrumentCode = "159509")
{
    return etfdt::accounting::TradeFactDto{
        factId,
        "2026-01-02T09:30:00Z",
        accountId,
        portfolioId,
        instrumentCode,
        etfdt::accounting::TradeAction::Buy,
        quantityText,
        "1.0000",
        amountText,
        feeText,
        "-" + amountText,
        currency,
        "unit-test",
        "multi-account buy",
    };
}

etfdt::accounting::TradeFactDto makeSell(const std::string& accountId = "ACC-DEMO-001")
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-SELL-" + accountId,
        "2026-01-03T09:30:00Z",
        accountId,
        "PORT-DEMO-001",
        "159509",
        etfdt::accounting::TradeAction::Sell,
        "100",
        "1.2000",
        "120.00",
        "1.00",
        "120.00",
        "CNY",
        "unit-test",
        "multi-account sell",
    };
}

std::vector<etfdt::accounting::CashFactDto> makeMultiAccountCashFacts()
{
    return {
        makeInitialCash("ACC-DEMO-001"),
        makeInitialCash("ACC-DEMO-002"),
    };
}

std::vector<etfdt::accounting::TradeFactDto> makeMultiAccountBuyFacts()
{
    return {
        makeBuy("TRADE-ACC-001", "ACC-DEMO-001", "1000", "1000.00"),
        makeBuy("TRADE-ACC-002", "ACC-DEMO-002", "2000", "2000.00"),
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
                              {makeBuy("TRADE-001", "ACC-DEMO-001", "1000", "1000.00")},
                              {makeInitialCash("ACC-DEMO-001", "100000.00")},
                              {},
                              {});
    failures += expect(singleBuyResult.status == AccountingReplayStatus::Ok, "single buy should still pass");

    const auto partialSellResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "ACC-DEMO-001", "1000", "1000.00"), makeSell()},
                              {makeInitialCash("ACC-DEMO-001", "100000.00")},
                              {},
                              {});
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell should still pass");

    auto sellExceedsTrade = makeSell();
    sellExceedsTrade.quantityText = "1200";
    const auto sellExceedsResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "ACC-DEMO-001", "1000", "1000.00"), sellExceedsTrade},
                              {makeInitialCash("ACC-DEMO-001", "100000.00")},
                              {},
                              {});
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Error,
                       "sell exceeds position should still pass");

    const auto missingFeeResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "ACC-DEMO-001", "1000", "1000.00", "")},
                              {makeInitialCash("ACC-DEMO-001", "100000.00")},
                              {},
                              {});
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning, "missing fee should still pass");
    failures += expect(hasIssueCode(missingFeeResult, AccountingIssueCode::MissingFee),
                       "missing fee should still return MISSING_FEE");

    const auto negativeCashResult =
        engine.replayReadOnly(request,
                              {makeBuy("TRADE-001", "ACC-DEMO-001", "1000", "1000.00")},
                              {makeInitialCash("ACC-DEMO-001", "100.00")},
                              {},
                              {});
    failures += expect(negativeCashResult.status == AccountingReplayStatus::Error, "negative cash should still pass");
    failures += expect(hasIssueCode(negativeCashResult, AccountingIssueCode::NegativeCash),
                       "negative cash should still return NEGATIVE_CASH");

    const auto multiInstrumentResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-159509", "ACC-DEMO-001", "1000", "1000.00"),
                                  makeBuy("TRADE-518880", "ACC-DEMO-001", "2000", "2000.00", "1.00", "PORT-DEMO-001", "CNY", "518880"),
                              },
                              {makeInitialCash("ACC-DEMO-001", "100000.00")},
                              {},
                              {});
    failures += expect(multiInstrumentResult.status == AccountingReplayStatus::Ok,
                       "multi-instrument buy should still pass");

    const auto multiAccountResult =
        engine.replayReadOnly(request, makeMultiAccountBuyFacts(), makeMultiAccountCashFacts(), {}, {});
    failures += expect(multiAccountResult.implemented, "multi-account BUY should be implemented");
    failures += expect(multiAccountResult.replayExecuted, "multi-account BUY should execute replay");
    failures += expect(multiAccountResult.status == AccountingReplayStatus::Ok, "multi-account BUY status should be OK");
    failures += expect(multiAccountResult.positionList.positions.size() == 2,
                       "multi-account BUY should output two account-scoped positions");

    const auto* accountOnePosition = findPositionByAccount(multiAccountResult, "ACC-DEMO-001");
    const auto* accountTwoPosition = findPositionByAccount(multiAccountResult, "ACC-DEMO-002");
    failures += expect(accountOnePosition != nullptr, "multi-account BUY should contain ACC-DEMO-001 position");
    failures += expect(accountTwoPosition != nullptr, "multi-account BUY should contain ACC-DEMO-002 position");
    if (accountOnePosition != nullptr) {
        failures += expect(accountOnePosition->portfolioId == "PORT-DEMO-001", "ACC-DEMO-001 portfolioId should be preserved");
        failures += expect(accountOnePosition->instrumentCode == "159509", "ACC-DEMO-001 instrumentCode should be 159509");
        failures += expect(accountOnePosition->quantityText == "1000", "ACC-DEMO-001 quantityText should be independent");
        failures += expect(accountOnePosition->costAmountText == "1001.00 CNY", "ACC-DEMO-001 cost should be independent");
        failures += expect(accountOnePosition->costPriceText == "1.001 CNY", "ACC-DEMO-001 costPrice should be stable");
        failures += expect(accountOnePosition->marketValueText == "UNAVAILABLE", "ACC-DEMO-001 marketValue should be unavailable");
        failures += expect(accountOnePosition->unrealizedPnlText == "UNAVAILABLE", "ACC-DEMO-001 unrealized PnL should be unavailable");
    }
    if (accountTwoPosition != nullptr) {
        failures += expect(accountTwoPosition->portfolioId == "PORT-DEMO-001", "ACC-DEMO-002 portfolioId should be preserved");
        failures += expect(accountTwoPosition->instrumentCode == "159509", "ACC-DEMO-002 instrumentCode should be 159509");
        failures += expect(accountTwoPosition->quantityText == "2000", "ACC-DEMO-002 quantityText should be independent");
        failures += expect(accountTwoPosition->costAmountText == "2001.00 CNY", "ACC-DEMO-002 cost should be independent");
        failures += expect(accountTwoPosition->costPriceText == "1.001 CNY", "ACC-DEMO-002 costPrice should be stable");
        failures += expect(accountTwoPosition->marketValueText == "UNAVAILABLE", "ACC-DEMO-002 marketValue should be unavailable");
        failures += expect(accountTwoPosition->unrealizedPnlText == "UNAVAILABLE", "ACC-DEMO-002 unrealized PnL should be unavailable");
    }

    failures += expect(!multiAccountResult.hasCashSummary, "multi-account BUY should not merge cash into one summary");
    failures += expect(multiAccountResult.accountCashSummaries.size() == 2,
                       "multi-account BUY should output per-account cash summaries");
    const auto* accountOneCash = findCashByAccount(multiAccountResult, "ACC-DEMO-001");
    const auto* accountTwoCash = findCashByAccount(multiAccountResult, "ACC-DEMO-002");
    failures += expect(accountOneCash != nullptr, "multi-account BUY should contain ACC-DEMO-001 cash summary");
    failures += expect(accountTwoCash != nullptr, "multi-account BUY should contain ACC-DEMO-002 cash summary");
    if (accountOneCash != nullptr) {
        failures += expect(accountOneCash->portfolioId == "PORT-DEMO-001", "ACC-DEMO-001 cash portfolioId should be preserved");
        failures += expect(accountOneCash->cashBalanceText == "48999.00 CNY", "ACC-DEMO-001 cash should not be mixed");
    }
    if (accountTwoCash != nullptr) {
        failures += expect(accountTwoCash->portfolioId == "PORT-DEMO-001", "ACC-DEMO-002 cash portfolioId should be preserved");
        failures += expect(accountTwoCash->cashBalanceText == "47999.00 CNY", "ACC-DEMO-002 cash should not be mixed");
    }
    failures += expect(multiAccountResult.hasPortfolioPnl, "multi-account BUY should output portfolio pnl shell");
    failures += expect(multiAccountResult.portfolioPnl.totalAssetsText == "UNAVAILABLE",
                       "multi-account BUY should not fabricate market value");
    failures += expect(multiAccountResult.portfolioPnl.unrealizedPnlText == "UNAVAILABLE",
                       "multi-account BUY should not fabricate unrealized pnl");
    failures += expect(!multiAccountResult.hasBasePosition, "multi-account BUY should not output base position");
    failures += expect(!multiAccountResult.hasSniperPool, "multi-account BUY should not output sniper pool");

    const auto multiAccountWithSellResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-ACC-001", "ACC-DEMO-001", "1000", "1000.00"),
                                  makeSell("ACC-DEMO-002"),
                              },
                              makeMultiAccountCashFacts(),
                              {},
                              {});
    failures += expect(multiAccountWithSellResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "multi-account with SELL should remain unsupported");

    const auto usdResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-ACC-001", "ACC-DEMO-001", "1000", "1000.00"),
                                  makeBuy("TRADE-ACC-002", "ACC-DEMO-002", "2000", "2000.00", "1.00", "PORT-DEMO-001", "USD"),
                              },
                              makeMultiAccountCashFacts(),
                              {},
                              {});
    failures += expect(usdResult.status == AccountingReplayStatus::Error,
                       "multi-account with USD should be rejected by multi-currency handling");
    failures += expect(hasIssueCode(usdResult, AccountingIssueCode::MultiCurrencyUnsupported),
                       "multi-account with USD should return MULTI_CURRENCY_UNSUPPORTED");

    const auto missingCashResult =
        engine.replayReadOnly(request,
                              makeMultiAccountBuyFacts(),
                              {makeInitialCash("ACC-DEMO-001")},
                              {},
                              {});
    failures += expect(missingCashResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "accountId without initial cash should remain unsupported");

    const auto accountNegativeCashResult =
        engine.replayReadOnly(request,
                              makeMultiAccountBuyFacts(),
                              {makeInitialCash("ACC-DEMO-001"), makeInitialCash("ACC-DEMO-002", "100.00")},
                              {},
                              {});
    failures += expect(accountNegativeCashResult.status == AccountingReplayStatus::Error,
                       "multi-account negative cash should return ERROR");
    failures += expect(hasIssueCode(accountNegativeCashResult, AccountingIssueCode::NegativeCash),
                       "multi-account negative cash should return NEGATIVE_CASH");

    const auto multiAccountMissingFeeResult =
        engine.replayReadOnly(request,
                              {
                                  makeBuy("TRADE-ACC-001", "ACC-DEMO-001", "1000", "1000.00"),
                                  makeBuy("TRADE-ACC-002", "ACC-DEMO-002", "2000", "2000.00", ""),
                              },
                              makeMultiAccountCashFacts(),
                              {},
                              {});
    failures += expect(multiAccountMissingFeeResult.status == AccountingReplayStatus::Warning,
                       "multi-account missing fee should return MISSING_FEE warning");
    failures += expect(hasIssueCode(multiAccountMissingFeeResult, AccountingIssueCode::MissingFee),
                       "multi-account missing fee should not default to zero");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.multiAccountBuyReplaySupported, "multiAccountBuyReplaySupported should be true");
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
