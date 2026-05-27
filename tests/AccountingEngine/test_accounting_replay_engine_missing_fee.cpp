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
        "missing fee test",
    };
}

etfdt::accounting::TradeFactDto makeBuy(const std::string& feeText = "",
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
        "1000.00",
        feeText,
        "-1000.00",
        currency,
        "unit-test",
        "missing fee buy",
    };
}

etfdt::accounting::TradeFactDto makeSell(const std::string& feeText = "")
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
        feeText,
        "480.00",
        "CNY",
        "unit-test",
        "missing fee sell",
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

    const auto singleBuyResult = engine.replayReadOnly(request, {makeBuy("1.00")}, {makeInitialCash()}, {}, {});
    failures += expect(singleBuyResult.status == AccountingReplayStatus::Ok, "single buy should still pass");

    const auto partialSellResult =
        engine.replayReadOnly(request, {makeBuy("1.00"), makeSell("1.00")}, {makeInitialCash()}, {}, {});
    failures += expect(partialSellResult.status == AccountingReplayStatus::Ok, "partial sell should still pass");

    const auto sellExceedsResult =
        engine.replayReadOnly(request, {makeBuy("1.00"), makeSell("1.00")}, {makeInitialCash("1000.00")}, {}, {});
    failures += expect(sellExceedsResult.status == AccountingReplayStatus::Ok,
                       "same facts with adequate position should not be sell exceeds");

    const auto missingFeeResult = engine.replayReadOnly(request, {makeBuy("")}, {makeInitialCash()}, {}, {});
    failures += expect(missingFeeResult.implemented, "missing fee should be implemented");
    failures += expect(missingFeeResult.replayExecuted, "missing fee should execute replay");
    failures += expect(missingFeeResult.status == AccountingReplayStatus::Warning, "missing fee status should be WARNING");
    failures += expect(missingFeeResult.message == "Trade fee is missing.", "missing fee message should be stable");
    failures += expect(missingFeeResult.issues.size() == 1, "missing fee should have one issue");
    if (!missingFeeResult.issues.empty()) {
        const auto& issue = missingFeeResult.issues.front();
        failures += expect(issue.code == AccountingIssueCode::MissingFee,
                           "missing fee issue code should be MISSING_FEE");
        failures += expect(issue.level == AccountingIssueLevel::Warning, "missing fee issue level should be WARNING");
        failures += expect(!issue.blocking, "missing fee issue should not be blocking");
    }
    failures += expect(missingFeeResult.positionList.positions.empty(), "missing fee should not output success positions");
    failures += expect(!missingFeeResult.hasCashSummary, "missing fee should not output cash summary");
    failures += expect(!missingFeeResult.hasPortfolioPnl, "missing fee should not output portfolio PnL");
    failures += expect(!missingFeeResult.hasBasePosition, "missing fee should not output base position");
    failures += expect(!missingFeeResult.hasSniperPool, "missing fee should not output sniper pool");

    const auto unavailableFeeResult = engine.replayReadOnly(request, {makeBuy("UNAVAILABLE")}, {makeInitialCash()}, {}, {});
    failures += expect(unavailableFeeResult.status == AccountingReplayStatus::Warning,
                       "unavailable fee should return MISSING_FEE warning");
    failures += expect(!unavailableFeeResult.issues.empty() &&
                           unavailableFeeResult.issues.front().code == AccountingIssueCode::MissingFee,
                       "unavailable fee should not default to zero");

    const auto zeroFeeResult = engine.replayReadOnly(request, {makeBuy("0.00")}, {makeInitialCash()}, {}, {});
    failures += expect(zeroFeeResult.status == AccountingReplayStatus::Ok, "zero fee text should not trigger missing fee");
    failures += expect(zeroFeeResult.issues.empty(), "zero fee text should not produce missing fee issue");
    failures += expect(!zeroFeeResult.positionList.positions.empty() &&
                           zeroFeeResult.positionList.positions.front().costAmountText == "1000.00 CNY",
                       "zero fee should be explicit and not treated as missing");

    const auto sellMissingFeeResult = engine.replayReadOnly(request, {makeSell("")}, {makeInitialCash()}, {}, {});
    failures += expect(sellMissingFeeResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "SELL missing fee should not default to zero");

    const auto unsupportedCurrencyResult =
        engine.replayReadOnly(request, {makeBuy("", "1000", "USD")}, {makeInitialCash("100000.00", "USD")}, {}, {});
    failures += expect(unsupportedCurrencyResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "USD missing fee should remain unsupported");

    const auto invalidQuantityResult = engine.replayReadOnly(request, {makeBuy("1.00", "many")}, {makeInitialCash()}, {}, {});
    failures += expect(invalidQuantityResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid quantity should return INVALID_REQUEST");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.missingFeeDetectionSupported, "missingFeeDetectionSupported should be true");
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
