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

struct QuantityParseResult {
    bool valid = false;
    long long microUnits = 0;
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

QuantityParseResult parseQuantityToMicroUnits(const std::string& text)
{
    if (text.empty()) {
        return {};
    }

    std::size_t index = 0;
    if (index >= text.size() || std::isdigit(static_cast<unsigned char>(text[index])) == 0) {
        return {};
    }

    long long units = 0;
    while (index < text.size() && std::isdigit(static_cast<unsigned char>(text[index])) != 0) {
        units = units * 10 + (text[index] - '0');
        ++index;
    }

    long long fraction = 0;
    int fractionDigits = 0;
    if (index < text.size() && text[index] == '.') {
        ++index;
        while (index < text.size() && std::isdigit(static_cast<unsigned char>(text[index])) != 0) {
            if (fractionDigits >= 6) {
                return {};
            }
            fraction = fraction * 10 + (text[index] - '0');
            ++fractionDigits;
            ++index;
        }
    }

    if (index != text.size()) {
        return {};
    }

    while (fractionDigits < 6) {
        fraction *= 10;
        ++fractionDigits;
    }

    return QuantityParseResult{true, units * 1000000 + fraction};
}

std::string formatQuantityFromMicroUnits(long long microUnits)
{
    const auto units = microUnits / 1000000;
    auto fraction = microUnits % 1000000;
    if (fraction == 0) {
        return std::to_string(units);
    }

    std::string fractionText = std::to_string(fraction + 1000000).substr(1);
    while (!fractionText.empty() && fractionText.back() == '0') {
        fractionText.pop_back();
    }
    return std::to_string(units) + "." + fractionText;
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

AccountingReplayResult makePartialSellInvalidRequest(std::vector<AccountingIssueDto> issues)
{
    return makeInvalidReplayRequestResult(std::move(issues));
}

AccountingReplayResult tryReplayBuySellPartial(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    if (!marketPriceFacts.empty() || !fxRateFacts.empty() || tradeFacts.size() != 2 || cashFacts.size() != 1) {
        return makeUnsupportedReplayScenarioResult();
    }

    const TradeFactDto* buyFact = nullptr;
    const TradeFactDto* sellFact = nullptr;
    for (const auto& tradeFact : tradeFacts) {
        if (tradeFact.action == TradeAction::Buy && buyFact == nullptr) {
            buyFact = &tradeFact;
        } else if (tradeFact.action == TradeAction::Sell && sellFact == nullptr) {
            sellFact = &tradeFact;
        } else {
            return makeUnsupportedReplayScenarioResult();
        }
    }

    if (buyFact == nullptr || sellFact == nullptr) {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto& cashFact = cashFacts.front();

    std::vector<AccountingIssueDto> validationIssues;
    const auto buyValidation = validateTradeFact(*buyFact);
    const auto sellValidation = validateTradeFact(*sellFact);
    const auto cashValidation = validateCashFact(cashFact);
    appendIssues(validationIssues, buyValidation.issues);
    appendIssues(validationIssues, sellValidation.issues);
    appendIssues(validationIssues, cashValidation.issues);

    if (buyFact->feeText.empty()) {
        validationIssues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::MissingRequiredField,
            "BUY feeText is required for partial sell replay.",
            true,
            "feeText",
            buyFact->factId));
    }
    if (sellFact->feeText.empty()) {
        validationIssues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::MissingRequiredField,
            "SELL feeText is required for partial sell replay.",
            true,
            "feeText",
            sellFact->factId));
    }

    if (!hasNonZeroQuantity(buyFact->quantityText)) {
        validationIssues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::InvalidQuantityText,
            "BUY quantityText must be greater than zero.",
            true,
            "quantityText",
            buyFact->factId));
    }
    if (!hasNonZeroQuantity(sellFact->quantityText)) {
        validationIssues.push_back(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::InvalidQuantityText,
            "SELL quantityText must be greater than zero.",
            true,
            "quantityText",
            sellFact->factId));
    }

    if (!validationIssues.empty()) {
        return makePartialSellInvalidRequest(std::move(validationIssues));
    }

    if (cashFact.action != CashAction::InitialCash) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (buyFact->accountId != sellFact->accountId || buyFact->portfolioId != sellFact->portfolioId ||
        buyFact->instrumentCode != sellFact->instrumentCode) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (buyFact->accountId != cashFact.accountId || buyFact->portfolioId != cashFact.portfolioId) {
        return makeUnsupportedReplayScenarioResult();
    }

    if ((!request.accountId.empty() && request.accountId != buyFact->accountId) ||
        (!request.portfolioId.empty() && request.portfolioId != buyFact->portfolioId)) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (buyFact->currency != "CNY" || sellFact->currency != "CNY" || cashFact.currency != "CNY") {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto initialCash = parseMoneyToCents(cashFact.amountText);
    const auto buyAmount = parseMoneyToCents(buyFact->amountText);
    const auto buyFee = parseMoneyToCents(buyFact->feeText);
    const auto sellAmount = parseMoneyToCents(sellFact->amountText);
    const auto sellFee = parseMoneyToCents(sellFact->feeText);
    const auto buyQuantity = parseQuantityToMicroUnits(buyFact->quantityText);
    const auto sellQuantity = parseQuantityToMicroUnits(sellFact->quantityText);
    if (!initialCash.valid || !buyAmount.valid || !buyFee.valid || !sellAmount.valid || !sellFee.valid ||
        !buyQuantity.valid || !sellQuantity.valid) {
        std::vector<AccountingIssueDto> issues;
        if (!initialCash.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "initial cash amount is not valid for partial sell replay.",
                true,
                "amountText",
                cashFact.factId));
        }
        if (!buyAmount.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY amount is not valid for partial sell replay.",
                true,
                "amountText",
                buyFact->factId));
        }
        if (!buyFee.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY fee is not valid for partial sell replay.",
                true,
                "feeText",
                buyFact->factId));
        }
        if (!sellAmount.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "SELL amount is not valid for partial sell replay.",
                true,
                "amountText",
                sellFact->factId));
        }
        if (!sellFee.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "SELL fee is not valid for partial sell replay.",
                true,
                "feeText",
                sellFact->factId));
        }
        if (!buyQuantity.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantity is not valid for partial sell replay.",
                true,
                "quantityText",
                buyFact->factId));
        }
        if (!sellQuantity.valid) {
            issues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "SELL quantity is not valid for partial sell replay.",
                true,
                "quantityText",
                sellFact->factId));
        }
        return makePartialSellInvalidRequest(std::move(issues));
    }

    if (sellQuantity.microUnits > buyQuantity.microUnits) {
        return makeSellExceedsPositionReplayResult();
    }
    if (sellQuantity.microUnits == buyQuantity.microUnits) {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto buyCostCents = buyAmount.cents + buyFee.cents;
    const auto allocatedSoldCostCents =
        (buyCostCents * sellQuantity.microUnits + buyQuantity.microUnits / 2) / buyQuantity.microUnits;
    const auto netSellInflowCents = sellAmount.cents - sellFee.cents;
    const auto realizedPnlCents = netSellInflowCents - allocatedSoldCostCents;
    const auto remainingQuantity = buyQuantity.microUnits - sellQuantity.microUnits;
    const auto remainingCostCents = buyCostCents - allocatedSoldCostCents;
    const auto cashBalanceCents = initialCash.cents - buyCostCents + netSellInflowCents;
    if (cashBalanceCents < 0) {
        return makeNegativeCashReplayResult();
    }

    return makeBuySellPartialReplayResult(
        buyFact->accountId,
        buyFact->portfolioId,
        buyFact->instrumentCode,
        formatQuantityFromMicroUnits(remainingQuantity),
        remainingCostCents,
        cashBalanceCents,
        realizedPnlCents);
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

    if (tradeFacts.size() == 1) {
        return tryReplaySingleBuy(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
    }

    if (tradeFacts.size() == 2) {
        return tryReplayBuySellPartial(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
    }

    return makeUnsupportedReplayScenarioResult();
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
