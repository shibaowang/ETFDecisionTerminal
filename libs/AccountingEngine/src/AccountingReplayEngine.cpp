#include "AccountingEngine/AccountingReplayEngine.h"

#include "AccountingEngine/AccountingReplayValidation.h"

#include <cctype>
#include <utility>

namespace etfdt::accounting {
namespace {

struct MoneyParseResult {
    bool valid = false;
    long long cents = 0;
};

bool hasNonZeroQuantity(const std::string& text)
{
    return text.find_first_not_of("0.") != std::string::npos;
}

MoneyParseResult parseMoneyToCents(const std::string& text)
{
    if (text.empty()) {
        return {};
    }

    std::size_t index = 0;
    bool negative = false;
    if (text[index] == '-') {
        negative = true;
        ++index;
    }

    if (index >= text.size() || std::isdigit(static_cast<unsigned char>(text[index])) == 0) {
        return {};
    }

    long long units = 0;
    while (index < text.size() && std::isdigit(static_cast<unsigned char>(text[index])) != 0) {
        units = units * 10 + (text[index] - '0');
        ++index;
    }

    long long cents = 0;
    int centDigits = 0;
    if (index < text.size() && text[index] == '.') {
        ++index;
        while (index < text.size() && std::isdigit(static_cast<unsigned char>(text[index])) != 0) {
            if (centDigits >= 2) {
                return {};
            }
            cents = cents * 10 + (text[index] - '0');
            ++centDigits;
            ++index;
        }
    }

    if (index != text.size()) {
        return {};
    }

    while (centDigits < 2) {
        cents *= 10;
        ++centDigits;
    }

    const auto total = units * 100 + cents;
    return MoneyParseResult{true, negative ? -total : total};
}

void appendIssues(std::vector<AccountingIssueDto>& target, const std::vector<AccountingIssueDto>& source)
{
    target.insert(target.end(), source.begin(), source.end());
}

AccountingReplayResult makeSingleBuyInvalidRequest(std::vector<AccountingIssueDto> issues)
{
    return makeInvalidReplayRequestResult(std::move(issues));
}

AccountingReplayResult tryReplaySingleBuy(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    if (!marketPriceFacts.empty() || !fxRateFacts.empty() || tradeFacts.size() != 1 || cashFacts.size() != 1) {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto& tradeFact = tradeFacts.front();
    const auto& cashFact = cashFacts.front();

    std::vector<AccountingIssueDto> validationIssues;
    const auto tradeValidation = validateTradeFact(tradeFact);
    const auto cashValidation = validateCashFact(cashFact);
    appendIssues(validationIssues, tradeValidation.issues);
    appendIssues(validationIssues, cashValidation.issues);

    if (tradeFact.feeText.empty()) {
        validationIssues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::MissingRequiredField,
            "feeText is required for single buy replay.",
            true,
            "feeText",
            tradeFact.factId));
    }

    if (!hasNonZeroQuantity(tradeFact.quantityText)) {
        validationIssues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::InvalidQuantityText,
            "quantityText must be greater than zero.",
            true,
            "quantityText",
            tradeFact.factId));
    }

    if (!validationIssues.empty()) {
        return makeSingleBuyInvalidRequest(std::move(validationIssues));
    }

    if (tradeFact.action != TradeAction::Buy || cashFact.action != CashAction::InitialCash) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (tradeFact.accountId != cashFact.accountId || tradeFact.portfolioId != cashFact.portfolioId) {
        return makeUnsupportedReplayScenarioResult();
    }

    if ((!request.accountId.empty() && request.accountId != tradeFact.accountId) ||
        (!request.portfolioId.empty() && request.portfolioId != tradeFact.portfolioId)) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (tradeFact.currency != "CNY" || cashFact.currency != "CNY") {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto initialCash = parseMoneyToCents(cashFact.amountText);
    const auto amount = parseMoneyToCents(tradeFact.amountText);
    const auto fee = parseMoneyToCents(tradeFact.feeText);
    if (!initialCash.valid || !amount.valid || !fee.valid) {
        std::vector<AccountingIssueDto> issues;
        if (!initialCash.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "initial cash amount is not valid for single buy replay.",
                true,
                "amountText",
                cashFact.factId));
        }
        if (!amount.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "trade amount is not valid for single buy replay.",
                true,
                "amountText",
                tradeFact.factId));
        }
        if (!fee.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "trade fee is not valid for single buy replay.",
                true,
                "feeText",
                tradeFact.factId));
        }
        return makeSingleBuyInvalidRequest(std::move(issues));
    }

    const auto costCents = amount.cents + fee.cents;
    const auto cashBalanceCents = initialCash.cents - costCents;
    if (cashBalanceCents < 0) {
        return makeNegativeCashReplayResult();
    }

    return makeSingleBuyReplayResult(
        tradeFact.accountId,
        tradeFact.portfolioId,
        tradeFact.instrumentCode,
        tradeFact.quantityText,
        costCents,
        cashBalanceCents);
}

} // namespace

AccountingReplayResult AccountingReplayEngine::replayReadOnly(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts) const
{
    const auto validation = validateReplayRequest(request);
    if (!validation.valid) {
        return makeInvalidReplayRequestResult(validation.issues);
    }

    if (tradeFacts.empty() && cashFacts.empty() && marketPriceFacts.empty() && fxRateFacts.empty()) {
        return makeEmptyLedgerReplayResult(request.accountId, request.portfolioId);
    }

    return tryReplaySingleBuy(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
}

AccountingReplayResult replayReadOnly(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    return AccountingReplayEngine{}.replayReadOnly(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
}

} // namespace etfdt::accounting
