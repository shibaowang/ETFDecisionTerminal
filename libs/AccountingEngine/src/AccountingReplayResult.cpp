#include "AccountingEngine/AccountingReplayResult.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace etfdt::accounting {
namespace {

std::string formatCents(long long cents)
{
    const bool negative = cents < 0;
    const auto absolute = negative ? -cents : cents;
    std::ostringstream stream;
    if (negative) {
        stream << '-';
    }
    stream << (absolute / 100) << '.' << std::setw(2) << std::setfill('0') << (absolute % 100) << " CNY";
    return stream.str();
}

std::string formatCostPrice(long long costCents, const std::string& quantityText)
{
    std::string whole;
    std::string fraction;
    const auto dot = quantityText.find('.');
    if (dot == std::string::npos) {
        whole = quantityText;
    } else {
        whole = quantityText.substr(0, dot);
        fraction = quantityText.substr(dot + 1);
    }

    long long quantityUnits = 0;
    for (const auto ch : whole) {
        quantityUnits = quantityUnits * 10 + (ch - '0');
    }

    long long scale = 1;
    for (const auto ch : fraction) {
        quantityUnits = quantityUnits * 10 + (ch - '0');
        scale *= 10;
    }

    const auto numerator = costCents * scale * 10;
    const auto roundedMilli = (numerator + quantityUnits / 2) / quantityUnits;
    std::ostringstream stream;
    stream << (roundedMilli / 1000) << '.' << std::setw(3) << std::setfill('0') << (roundedMilli % 1000) << " CNY";
    return stream.str();
}

} // namespace

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

AccountingReplayResult makeSingleBuyReplayResult(
    const std::string& accountId,
    const std::string& portfolioId,
    const std::string& instrumentCode,
    const std::string& quantityText,
    long long costCents,
    long long cashBalanceCents)
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Ok;
    result.message = "Single buy replay completed.";
    result.positionList.dataQualityStatus = "OK";
    result.positionList.positions.push_back(PositionSummaryDto{
        accountId,
        portfolioId,
        instrumentCode,
        quantityText,
        formatCents(costCents),
        formatCostPrice(costCents, quantityText),
        "UNAVAILABLE",
        "UNAVAILABLE",
        "CNY",
        "OK",
    });
    result.hasCashSummary = true;
    result.cashSummary.accountId = accountId;
    result.cashSummary.portfolioId = portfolioId;
    result.cashSummary.currency = "CNY";
    result.cashSummary.cashBalanceText = formatCents(cashBalanceCents);
    result.cashSummary.dataQualityStatus = "OK";
    result.hasPortfolioPnl = true;
    result.portfolioPnl.portfolioId = portfolioId;
    result.portfolioPnl.currency = "CNY";
    result.portfolioPnl.realizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.unrealizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.totalAssetsText = "UNAVAILABLE";
    result.portfolioPnl.totalPnlText = "UNAVAILABLE";
    result.portfolioPnl.dataQualityStatus = "UNAVAILABLE";
    return result;
}

AccountingReplayResult makeBuySellPartialReplayResult(
    const std::string& accountId,
    const std::string& portfolioId,
    const std::string& instrumentCode,
    const std::string& remainingQuantityText,
    long long remainingCostCents,
    long long cashBalanceCents,
    long long realizedPnlCents)
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Ok;
    result.message = "Buy-sell partial replay completed.";
    result.positionList.dataQualityStatus = "OK";
    result.positionList.positions.push_back(PositionSummaryDto{
        accountId,
        portfolioId,
        instrumentCode,
        remainingQuantityText,
        formatCents(remainingCostCents),
        formatCostPrice(remainingCostCents, remainingQuantityText),
        "UNAVAILABLE",
        "UNAVAILABLE",
        "CNY",
        "OK",
    });
    result.hasCashSummary = true;
    result.cashSummary.accountId = accountId;
    result.cashSummary.portfolioId = portfolioId;
    result.cashSummary.currency = "CNY";
    result.cashSummary.cashBalanceText = formatCents(cashBalanceCents);
    result.cashSummary.dataQualityStatus = "OK";
    result.hasPortfolioPnl = true;
    result.portfolioPnl.portfolioId = portfolioId;
    result.portfolioPnl.currency = "CNY";
    result.portfolioPnl.realizedPnlText = formatCents(realizedPnlCents);
    result.portfolioPnl.unrealizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.totalAssetsText = "UNAVAILABLE";
    result.portfolioPnl.totalPnlText = "UNAVAILABLE";
    result.portfolioPnl.dataQualityStatus = "UNAVAILABLE";
    return result;
}

AccountingReplayResult makeMultiInstrumentBuyReplayResult(
    const std::string& accountId,
    const std::string& portfolioId,
    std::vector<PositionSummaryDto> positions,
    long long cashBalanceCents)
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Ok;
    result.message = "Multi-instrument buy replay completed.";
    result.positionList.dataQualityStatus = "OK";
    result.positionList.positions = std::move(positions);
    result.hasCashSummary = true;
    result.cashSummary.accountId = accountId;
    result.cashSummary.portfolioId = portfolioId;
    result.cashSummary.currency = "CNY";
    result.cashSummary.cashBalanceText = formatCents(cashBalanceCents);
    result.cashSummary.dataQualityStatus = "OK";
    result.hasPortfolioPnl = true;
    result.portfolioPnl.portfolioId = portfolioId;
    result.portfolioPnl.currency = "CNY";
    result.portfolioPnl.realizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.unrealizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.totalAssetsText = "UNAVAILABLE";
    result.portfolioPnl.totalPnlText = "UNAVAILABLE";
    result.portfolioPnl.dataQualityStatus = "UNAVAILABLE";
    return result;
}

AccountingReplayResult makeMultiAccountBuyReplayResult(
    const std::string& portfolioId,
    std::vector<PositionSummaryDto> positions,
    std::vector<CashSummaryDto> accountCashSummaries)
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Ok;
    result.message = "Multi-account buy replay completed.";
    result.positionList.dataQualityStatus = "OK";
    result.positionList.positions = std::move(positions);
    result.accountCashSummaries = std::move(accountCashSummaries);
    result.hasPortfolioPnl = true;
    result.portfolioPnl.portfolioId = portfolioId;
    result.portfolioPnl.currency = "CNY";
    result.portfolioPnl.realizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.unrealizedPnlText = "UNAVAILABLE";
    result.portfolioPnl.totalAssetsText = "UNAVAILABLE";
    result.portfolioPnl.totalPnlText = "UNAVAILABLE";
    result.portfolioPnl.dataQualityStatus = "UNAVAILABLE";
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
    result.message = "The requested replay scenario is not supported by this skeleton.";
    result.issues.push_back(makeAccountingIssue(
        AccountingIssueLevel::Warning,
        AccountingIssueCode::ReplayNotImplemented,
        "Non-empty replay scenarios are not implemented.",
        false,
        "facts"));
    return result;
}

AccountingReplayResult makeNegativeCashReplayResult()
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Error;
    result.message = "Single buy cash requirement exceeds available cash.";
    result.issues.push_back(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::NegativeCash,
        "Buy cash requirement exceeds initial cash.",
        true,
        "cashFacts"));
    return result;
}

AccountingReplayResult makeSellExceedsPositionReplayResult()
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Error;
    result.message = "Sell quantity exceeds available position.";
    result.issues.push_back(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::SellExceedsPosition,
        "Sell quantity exceeds bought quantity.",
        true,
        "tradeFacts"));
    return result;
}

AccountingReplayResult makeMissingFeeReplayResult()
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Warning;
    result.message = "Trade fee is missing.";
    result.issues.push_back(makeAccountingIssue(
        AccountingIssueLevel::Warning,
        AccountingIssueCode::MissingFee,
        "Trade fee is missing.",
        false,
        "feeText"));
    return result;
}

AccountingReplayResult makeMultiCurrencyUnsupportedReplayResult(bool fxRateMissing)
{
    AccountingReplayResult result;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = AccountingReplayStatus::Error;
    result.message = fxRateMissing ? "Multi-currency replay requires FX rate support."
                                   : "Multi-currency replay is not supported by this skeleton.";
    result.issues.push_back(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::MultiCurrencyUnsupported,
        "Multi-currency replay is not supported.",
        true,
        "currency"));
    if (fxRateMissing) {
        result.issues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::FxRateMissing,
            "FX rate facts are missing for multi-currency replay.",
            true,
            "fxRateFacts"));
    }
    return result;
}

} // namespace etfdt::accounting
