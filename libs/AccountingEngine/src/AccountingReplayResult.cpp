#include "AccountingEngine/AccountingReplayResult.h"

#include <utility>

namespace etfdt::accounting {

AccountingReplayResult makeEmptyLedgerReplayResult(const std::string& accountId, const std::string& portfolioId)
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Ok;
    result.message = "Empty ledger replay completed.";
    result.positionList.dataQualityStatus = "OK";
    result.hasCashSummary = true;
    result.cashSummary.accountId = accountId;
    result.cashSummary.portfolioId = portfolioId;
    result.cashSummary.currency = "CNY";
    result.cashSummary.cashBalanceText = "0.00 CNY";
    result.cashSummary.dataQualityStatus = "OK";
    result.hasPortfolioPnl = true;
    result.portfolioPnl.portfolioId = portfolioId;
    result.portfolioPnl.currency = "CNY";
    result.portfolioPnl.totalAssetsText = "0.00 CNY";
    result.portfolioPnl.totalPnlText = "0.00 CNY";
    result.portfolioPnl.dataQualityStatus = "OK";
    return result;
}

AccountingReplayResult makeInvalidReplayRequestResult(std::vector<AccountingIssueDto> issues)
{
    AccountingReplayResult result;
    result.implemented = false;
    result.replayExecuted = false;
    result.status = AccountingReplayStatus::InvalidRequest;
    result.message = "Replay request validation failed.";
    result.issues = std::move(issues);
    return result;
}

AccountingReplayResult makeUnsupportedReplayScenarioResult()
{
    AccountingReplayResult result;
    result.implemented = false;
    result.replayExecuted = false;
    result.status = AccountingReplayStatus::UnsupportedScenario;
    result.message = "Only empty ledger replay is supported by this skeleton.";
    result.issues.push_back(makeAccountingIssue(
        AccountingIssueLevel::Warning,
        AccountingIssueCode::ReplayNotImplemented,
        "Non-empty replay scenarios are not implemented.",
        false,
        "facts"));
    return result;
}

} // namespace etfdt::accounting
