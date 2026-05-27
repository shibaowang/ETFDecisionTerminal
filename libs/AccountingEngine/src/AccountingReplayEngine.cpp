#include "AccountingEngine/AccountingReplayEngine.h"

#include "AccountingEngine/AccountingReplayValidation.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
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

bool isMissingFeeText(const std::string& text)
{
    return text.empty() || text == "UNAVAILABLE" || text == "N/A" || text == "NA" || text == "null";
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

std::string formatCentsForReplay(long long cents)
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

std::string formatCostPriceForReplay(long long costCents, const std::string& quantityText)
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
    auto tradeFactForValidation = tradeFact;
    if (isMissingFeeText(tradeFactForValidation.feeText)) {
        tradeFactForValidation.feeText.clear();
    }
    const auto tradeValidation = validateTradeFact(tradeFactForValidation);
    const auto cashValidation = validateCashFact(cashFact);
    appendIssues(validationIssues, tradeValidation.issues);
    appendIssues(validationIssues, cashValidation.issues);

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

    if (isMissingFeeText(tradeFact.feeText)) {
        return makeMissingFeeReplayResult();
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

struct InstrumentBuyAggregate {
    long long quantityMicroUnits = 0;
    long long costCents = 0;
};

struct AccountInstrumentKey {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;

    bool operator<(const AccountInstrumentKey& other) const
    {
        if (accountId != other.accountId) {
            return accountId < other.accountId;
        }
        if (portfolioId != other.portfolioId) {
            return portfolioId < other.portfolioId;
        }
        return instrumentCode < other.instrumentCode;
    }
};

AccountingReplayResult tryReplayMultiInstrumentBuy(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    if (!marketPriceFacts.empty() || !fxRateFacts.empty() || tradeFacts.size() < 2 || cashFacts.size() != 1) {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto& cashFact = cashFacts.front();

    std::vector<AccountingIssueDto> validationIssues;
    const auto cashValidation = validateCashFact(cashFact);
    appendIssues(validationIssues, cashValidation.issues);

    for (const auto& tradeFact : tradeFacts) {
        auto tradeFactForValidation = tradeFact;
        if (isMissingFeeText(tradeFactForValidation.feeText)) {
            tradeFactForValidation.feeText.clear();
        }
        const auto tradeValidation = validateTradeFact(tradeFactForValidation);
        appendIssues(validationIssues, tradeValidation.issues);

        if (!hasNonZeroQuantity(tradeFact.quantityText)) {
            validationIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantityText must be greater than zero.",
                true,
                "quantityText",
                tradeFact.factId));
        }
    }

    if (!validationIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(validationIssues));
    }

    if (cashFact.action != CashAction::InitialCash) {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto& firstTrade = tradeFacts.front();
    if (firstTrade.action != TradeAction::Buy) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (firstTrade.accountId != cashFact.accountId || firstTrade.portfolioId != cashFact.portfolioId) {
        return makeUnsupportedReplayScenarioResult();
    }

    if ((!request.accountId.empty() && request.accountId != firstTrade.accountId) ||
        (!request.portfolioId.empty() && request.portfolioId != firstTrade.portfolioId)) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (cashFact.currency != "CNY") {
        return makeUnsupportedReplayScenarioResult();
    }

    std::set<std::string> instrumentCodes;
    for (const auto& tradeFact : tradeFacts) {
        if (tradeFact.action != TradeAction::Buy || tradeFact.accountId != firstTrade.accountId ||
            tradeFact.portfolioId != firstTrade.portfolioId || tradeFact.currency != "CNY") {
            return makeUnsupportedReplayScenarioResult();
        }

        instrumentCodes.insert(tradeFact.instrumentCode);
    }

    if (instrumentCodes.size() < 2) {
        return makeUnsupportedReplayScenarioResult();
    }

    for (const auto& tradeFact : tradeFacts) {
        if (isMissingFeeText(tradeFact.feeText)) {
            return makeMissingFeeReplayResult();
        }
    }

    const auto initialCash = parseMoneyToCents(cashFact.amountText);
    if (!initialCash.valid) {
        return makeInvalidReplayRequestResult({makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::InvalidMoneyText,
            "initial cash amount is not valid for multi-instrument buy replay.",
            true,
            "amountText",
            cashFact.factId)});
    }

    std::map<std::string, InstrumentBuyAggregate> aggregates;
    long long totalCostCents = 0;
    std::vector<AccountingIssueDto> parseIssues;
    for (const auto& tradeFact : tradeFacts) {
        const auto amount = parseMoneyToCents(tradeFact.amountText);
        const auto fee = parseMoneyToCents(tradeFact.feeText);
        const auto quantity = parseQuantityToMicroUnits(tradeFact.quantityText);
        if (!amount.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY amount is not valid for multi-instrument buy replay.",
                true,
                "amountText",
                tradeFact.factId));
        }
        if (!fee.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY fee is not valid for multi-instrument buy replay.",
                true,
                "feeText",
                tradeFact.factId));
        }
        if (!quantity.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantity is not valid for multi-instrument buy replay.",
                true,
                "quantityText",
                tradeFact.factId));
        }
        if (!amount.valid || !fee.valid || !quantity.valid) {
            continue;
        }

        const auto costCents = amount.cents + fee.cents;
        totalCostCents += costCents;
        auto& aggregate = aggregates[tradeFact.instrumentCode];
        aggregate.quantityMicroUnits += quantity.microUnits;
        aggregate.costCents += costCents;
    }

    if (!parseIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(parseIssues));
    }

    const auto cashBalanceCents = initialCash.cents - totalCostCents;
    if (cashBalanceCents < 0) {
        return makeNegativeCashReplayResult();
    }

    std::vector<PositionSummaryDto> positions;
    for (const auto& [instrumentCode, aggregate] : aggregates) {
        const auto quantityText = formatQuantityFromMicroUnits(aggregate.quantityMicroUnits);
        positions.push_back(PositionSummaryDto{
            firstTrade.accountId,
            firstTrade.portfolioId,
            instrumentCode,
            quantityText,
            formatCentsForReplay(aggregate.costCents),
            formatCostPriceForReplay(aggregate.costCents, quantityText),
            "UNAVAILABLE",
            "UNAVAILABLE",
            "CNY",
            "OK",
        });
    }

    return makeMultiInstrumentBuyReplayResult(firstTrade.accountId, firstTrade.portfolioId, std::move(positions), cashBalanceCents);
}

AccountingReplayResult tryReplayMultiAccountBuy(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    if (!marketPriceFacts.empty() || !fxRateFacts.empty() || tradeFacts.size() < 2 || cashFacts.size() < 2) {
        return makeUnsupportedReplayScenarioResult();
    }

    std::vector<AccountingIssueDto> validationIssues;
    for (const auto& cashFact : cashFacts) {
        const auto cashValidation = validateCashFact(cashFact);
        appendIssues(validationIssues, cashValidation.issues);
    }

    for (const auto& tradeFact : tradeFacts) {
        auto tradeFactForValidation = tradeFact;
        if (isMissingFeeText(tradeFactForValidation.feeText)) {
            tradeFactForValidation.feeText.clear();
        }
        const auto tradeValidation = validateTradeFact(tradeFactForValidation);
        appendIssues(validationIssues, tradeValidation.issues);

        if (!hasNonZeroQuantity(tradeFact.quantityText)) {
            validationIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantityText must be greater than zero.",
                true,
                "quantityText",
                tradeFact.factId));
        }
    }

    if (!validationIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(validationIssues));
    }

    const auto& firstCash = cashFacts.front();
    const auto commonPortfolioId = firstCash.portfolioId;
    if (!request.accountId.empty() || (!request.portfolioId.empty() && request.portfolioId != commonPortfolioId)) {
        return makeUnsupportedReplayScenarioResult();
    }

    if (firstCash.action != CashAction::InitialCash || firstCash.currency != "CNY") {
        return makeUnsupportedReplayScenarioResult();
    }

    std::map<std::string, long long> initialCashByAccount;
    std::set<std::string> accountIds;
    std::vector<AccountingIssueDto> parseIssues;
    for (const auto& cashFact : cashFacts) {
        if (cashFact.action != CashAction::InitialCash || cashFact.currency != "CNY" ||
            cashFact.portfolioId != commonPortfolioId) {
            return makeUnsupportedReplayScenarioResult();
        }
        if (initialCashByAccount.find(cashFact.accountId) != initialCashByAccount.end()) {
            return makeUnsupportedReplayScenarioResult();
        }

        const auto initialCash = parseMoneyToCents(cashFact.amountText);
        if (!initialCash.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "initial cash amount is not valid for multi-account buy replay.",
                true,
                "amountText",
                cashFact.factId));
            continue;
        }

        initialCashByAccount[cashFact.accountId] = initialCash.cents;
        accountIds.insert(cashFact.accountId);
    }

    if (!parseIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(parseIssues));
    }
    if (accountIds.size() < 2) {
        return makeUnsupportedReplayScenarioResult();
    }

    const auto& firstTrade = tradeFacts.front();
    if (firstTrade.action != TradeAction::Buy || firstTrade.currency != "CNY" ||
        firstTrade.portfolioId != commonPortfolioId) {
        return makeUnsupportedReplayScenarioResult();
    }
    const auto commonInstrumentCode = firstTrade.instrumentCode;

    for (const auto& tradeFact : tradeFacts) {
        if (tradeFact.action != TradeAction::Buy || tradeFact.currency != "CNY" ||
            tradeFact.portfolioId != commonPortfolioId || tradeFact.instrumentCode != commonInstrumentCode ||
            initialCashByAccount.find(tradeFact.accountId) == initialCashByAccount.end()) {
            return makeUnsupportedReplayScenarioResult();
        }
    }

    for (const auto& tradeFact : tradeFacts) {
        if (isMissingFeeText(tradeFact.feeText)) {
            return makeMissingFeeReplayResult();
        }
    }

    std::map<AccountInstrumentKey, InstrumentBuyAggregate> aggregates;
    std::map<std::string, long long> costByAccount;
    for (const auto& tradeFact : tradeFacts) {
        const auto amount = parseMoneyToCents(tradeFact.amountText);
        const auto fee = parseMoneyToCents(tradeFact.feeText);
        const auto quantity = parseQuantityToMicroUnits(tradeFact.quantityText);
        if (!amount.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY amount is not valid for multi-account buy replay.",
                true,
                "amountText",
                tradeFact.factId));
        }
        if (!fee.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY fee is not valid for multi-account buy replay.",
                true,
                "feeText",
                tradeFact.factId));
        }
        if (!quantity.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantity is not valid for multi-account buy replay.",
                true,
                "quantityText",
                tradeFact.factId));
        }
        if (!amount.valid || !fee.valid || !quantity.valid) {
            continue;
        }

        const auto costCents = amount.cents + fee.cents;
        costByAccount[tradeFact.accountId] += costCents;
        auto& aggregate = aggregates[AccountInstrumentKey{
            tradeFact.accountId,
            tradeFact.portfolioId,
            tradeFact.instrumentCode,
        }];
        aggregate.quantityMicroUnits += quantity.microUnits;
        aggregate.costCents += costCents;
    }

    if (!parseIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(parseIssues));
    }

    for (const auto& [accountId, costCents] : costByAccount) {
        if (initialCashByAccount[accountId] - costCents < 0) {
            return makeNegativeCashReplayResult();
        }
    }

    std::vector<PositionSummaryDto> positions;
    for (const auto& [key, aggregate] : aggregates) {
        const auto quantityText = formatQuantityFromMicroUnits(aggregate.quantityMicroUnits);
        positions.push_back(PositionSummaryDto{
            key.accountId,
            key.portfolioId,
            key.instrumentCode,
            quantityText,
            formatCentsForReplay(aggregate.costCents),
            formatCostPriceForReplay(aggregate.costCents, quantityText),
            "UNAVAILABLE",
            "UNAVAILABLE",
            "CNY",
            "OK",
        });
    }

    std::vector<CashSummaryDto> accountCashSummaries;
    for (const auto& [accountId, initialCashCents] : initialCashByAccount) {
        const auto cashBalanceCents = initialCashCents - costByAccount[accountId];
        accountCashSummaries.push_back(CashSummaryDto{
            accountId,
            commonPortfolioId,
            "CNY",
            formatCentsForReplay(cashBalanceCents),
            "OK",
        });
    }

    return makeMultiAccountBuyReplayResult(commonPortfolioId, std::move(positions), std::move(accountCashSummaries));
}

AccountingReplayResult tryReplayMultiCurrencyUnsupported(
    const ReplayRequestDto&,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>&,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    if (tradeFacts.size() < 2) {
        return makeUnsupportedReplayScenarioResult();
    }

    std::vector<AccountingIssueDto> validationIssues;
    for (const auto& cashFact : cashFacts) {
        const auto cashValidation = validateCashFact(cashFact);
        appendIssues(validationIssues, cashValidation.issues);
    }

    for (const auto& tradeFact : tradeFacts) {
        auto tradeFactForValidation = tradeFact;
        if (isMissingFeeText(tradeFactForValidation.feeText)) {
            tradeFactForValidation.feeText.clear();
        }
        const auto tradeValidation = validateTradeFact(tradeFactForValidation);
        appendIssues(validationIssues, tradeValidation.issues);

        if (!hasNonZeroQuantity(tradeFact.quantityText)) {
            validationIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantityText must be greater than zero.",
                true,
                "quantityText",
                tradeFact.factId));
        }
    }

    for (const auto& fxRateFact : fxRateFacts) {
        const auto fxValidation = validateFxRateFact(fxRateFact);
        appendIssues(validationIssues, fxValidation.issues);
    }

    if (!validationIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(validationIssues));
    }

    std::set<std::string> currencies;
    for (const auto& cashFact : cashFacts) {
        currencies.insert(cashFact.currency);
    }
    for (const auto& tradeFact : tradeFacts) {
        if (tradeFact.action != TradeAction::Buy) {
            return makeUnsupportedReplayScenarioResult();
        }
        currencies.insert(tradeFact.currency);
    }

    const auto hasCny = currencies.find("CNY") != currencies.end();
    const auto hasNonCny = std::any_of(currencies.begin(), currencies.end(), [](const auto& currency) {
        return currency != "CNY";
    });
    if (!hasCny || !hasNonCny) {
        return makeUnsupportedReplayScenarioResult();
    }

    for (const auto& tradeFact : tradeFacts) {
        if (isMissingFeeText(tradeFact.feeText)) {
            return makeMissingFeeReplayResult();
        }
    }

    std::vector<AccountingIssueDto> parseIssues;
    for (const auto& cashFact : cashFacts) {
        const auto amount = parseMoneyToCents(cashFact.amountText);
        if (!amount.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "cash amount is not valid for multi-currency replay detection.",
                true,
                "amountText",
                cashFact.factId));
        }
    }
    for (const auto& tradeFact : tradeFacts) {
        const auto amount = parseMoneyToCents(tradeFact.amountText);
        const auto fee = parseMoneyToCents(tradeFact.feeText);
        const auto quantity = parseQuantityToMicroUnits(tradeFact.quantityText);
        if (!amount.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY amount is not valid for multi-currency replay detection.",
                true,
                "amountText",
                tradeFact.factId));
        }
        if (!fee.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidMoneyText,
                "BUY fee is not valid for multi-currency replay detection.",
                true,
                "feeText",
                tradeFact.factId));
        }
        if (!quantity.valid) {
            parseIssues.push_back(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidQuantityText,
                "BUY quantity is not valid for multi-currency replay detection.",
                true,
                "quantityText",
                tradeFact.factId));
        }
    }
    if (!parseIssues.empty()) {
        return makeInvalidReplayRequestResult(std::move(parseIssues));
    }

    return fxRateFacts.empty() ? makeMultiCurrencyUnsupportedReplayResult(true)
                              : makeUnsupportedReplayScenarioResult();
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

    if (tradeFacts.size() >= 2) {
        const auto multiCurrencyResult =
            tryReplayMultiCurrencyUnsupported(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
        if (multiCurrencyResult.status != AccountingReplayStatus::UnsupportedScenario) {
            return multiCurrencyResult;
        }
    }

    if (tradeFacts.size() >= 2 && cashFacts.size() >= 2) {
        return tryReplayMultiAccountBuy(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
    }

    if (tradeFacts.size() == 2) {
        const auto partialSellResult =
            tryReplayBuySellPartial(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
        if (partialSellResult.status != AccountingReplayStatus::UnsupportedScenario) {
            return partialSellResult;
        }
        return tryReplayMultiInstrumentBuy(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
    }

    if (tradeFacts.size() > 2) {
        return tryReplayMultiInstrumentBuy(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
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
