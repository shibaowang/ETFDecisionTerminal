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

etfdt::accounting::TradeFactDto makeTradeFact()
{
    return etfdt::accounting::TradeFactDto{
        "TRADE-001",
        "2026-01-02T09:30:00Z",
        "ACC-DEMO-001",
        "PORT-DEMO-001",
        "159509",
        etfdt::accounting::TradeAction::Buy,
        "100",
        "1.2500",
        "125.00",
        "0.10",
        "-125.10",
        "CNY",
        "unit-test",
        "non-empty facts remain unsupported",
    };
}

} // namespace

int main()
{
    using namespace etfdt::accounting;

    int failures = 0;

    AccountingReplayEngine engine;

    ReplayRequestDto request;
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PORT-DEMO-001";
    request.requestedOutputs = {
        RequestedReplayOutput::Positions,
        RequestedReplayOutput::Cash,
        RequestedReplayOutput::Pnl,
        RequestedReplayOutput::Issues,
    };

    const auto emptyResult = engine.replayReadOnly(request, {}, {}, {}, {});
    failures += expect(emptyResult.implemented, "empty ledger should be implemented");
    failures += expect(emptyResult.replayExecuted, "empty ledger should execute replay");
    failures += expect(emptyResult.status == AccountingReplayStatus::Ok, "empty ledger status should be OK");
    failures += expect(emptyResult.issues.empty(), "empty ledger should not contain issues");
    failures += expect(emptyResult.positionList.positions.empty(), "empty ledger positions should be empty");
    failures += expect(emptyResult.positionList.dataQualityStatus == "OK", "empty ledger position data quality should be OK");
    failures += expect(emptyResult.hasCashSummary, "empty ledger should have cash summary");
    failures += expect(emptyResult.cashSummary.cashBalanceText == "0.00 CNY", "empty ledger cash balance should be 0.00 CNY");
    failures += expect(emptyResult.cashSummary.dataQualityStatus == "OK", "empty ledger cash data quality should be OK");
    failures += expect(emptyResult.hasPortfolioPnl, "empty ledger should have portfolio pnl");
    failures += expect(emptyResult.portfolioPnl.totalAssetsText == "0.00 CNY", "empty ledger total assets should be 0.00 CNY");
    failures += expect(emptyResult.portfolioPnl.totalPnlText == "0.00 CNY", "empty ledger total pnl should be 0.00 CNY");
    failures += expect(emptyResult.portfolioPnl.dataQualityStatus == "OK", "empty ledger pnl data quality should be OK");
    failures += expect(!emptyResult.hasBasePosition, "empty ledger should not contain base position");
    failures += expect(!emptyResult.hasSniperPool, "empty ledger should not contain sniper pool");

    const auto freeFunctionResult = replayReadOnly(request, {}, {}, {}, {});
    failures += expect(freeFunctionResult.status == AccountingReplayStatus::Ok, "free replayReadOnly should handle empty ledger");

    const auto nonEmptyTradeResult = engine.replayReadOnly(request, {makeTradeFact()}, {}, {}, {});
    failures += expect(!nonEmptyTradeResult.implemented, "non-empty trade facts should not be implemented");
    failures += expect(!nonEmptyTradeResult.replayExecuted, "non-empty trade facts should not execute replay");
    failures += expect(nonEmptyTradeResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "non-empty trade facts should return UNSUPPORTED_SCENARIO");
    failures += expect(!nonEmptyTradeResult.issues.empty(), "non-empty trade facts should return an issue");
    failures += expect(nonEmptyTradeResult.issues.front().code == AccountingIssueCode::ReplayNotImplemented,
                       "non-empty trade facts should return REPLAY_NOT_IMPLEMENTED issue");
    failures += expect(nonEmptyTradeResult.positionList.positions.empty(), "non-empty trade facts should not output positions");
    failures += expect(!nonEmptyTradeResult.hasCashSummary, "non-empty trade facts should not output cash summary");
    failures += expect(!nonEmptyTradeResult.hasPortfolioPnl, "non-empty trade facts should not output portfolio pnl");

    CashFactDto cashFact;
    cashFact.factId = "CASH-001";
    cashFact.time = "2026-01-01T00:00:00Z";
    cashFact.accountId = "ACC-DEMO-001";
    cashFact.portfolioId = "PORT-DEMO-001";
    cashFact.action = CashAction::InitialCash;
    cashFact.amountText = "1000.00";
    cashFact.currency = "CNY";
    const auto nonEmptyCashResult = engine.replayReadOnly(request, {}, {cashFact}, {}, {});
    failures += expect(nonEmptyCashResult.status == AccountingReplayStatus::UnsupportedScenario,
                       "non-empty cash facts should return UNSUPPORTED_SCENARIO");

    ReplayRequestDto invalidRequest;
    invalidRequest.sourceFromTime = "2026-02-01T00:00:00Z";
    invalidRequest.sourceToTime = "2026-01-01T00:00:00Z";
    const auto invalidResult = engine.replayReadOnly(invalidRequest, {}, {}, {}, {});
    failures += expect(!invalidResult.implemented, "invalid request should not be implemented");
    failures += expect(!invalidResult.replayExecuted, "invalid request should not execute replay");
    failures += expect(invalidResult.status == AccountingReplayStatus::InvalidRequest,
                       "invalid request should return INVALID_REQUEST");
    failures += expect(!invalidResult.issues.empty(), "invalid request should contain issues");
    failures += expect(invalidResult.issues.front().code == AccountingIssueCode::InvalidTimeRange,
                       "invalid request should include INVALID_TIME_RANGE");

    const auto info = accountingEngineInfo();
    failures += expect(!info.replayImplemented, "AccountingEngineInfo.replayImplemented should remain false");
    failures += expect(!info.productionReady, "AccountingEngineInfo.productionReady should remain false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should remain false");
    failures += expect(capabilities.emptyLedgerReplaySupported, "capabilities.emptyLedgerReplaySupported should be true");
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
