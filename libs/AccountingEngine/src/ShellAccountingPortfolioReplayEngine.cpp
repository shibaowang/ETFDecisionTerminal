#include "AccountingEngine/ShellAccountingPortfolioReplayEngine.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <map>
#include <set>
#include <sstream>
#include <tuple>
#include <type_traits>

namespace etfdt::accounting {
namespace {

constexpr long long kScale = 1000000LL;

struct DecimalParseResult {
    bool valid = false;
    long long value = 0;
};

struct AccountPortfolioCurrencyKey {
    std::string accountId;
    std::string portfolioId;
    std::string currency;

    [[nodiscard]] bool operator<(const AccountPortfolioCurrencyKey& other) const noexcept
    {
        return std::tie(accountId, portfolioId, currency) <
            std::tie(other.accountId, other.portfolioId, other.currency);
    }
};

struct PositionKey {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string currency;

    [[nodiscard]] bool operator<(const PositionKey& other) const noexcept
    {
        return std::tie(accountId, portfolioId, instrumentCode, currency) <
            std::tie(other.accountId, other.portfolioId, other.instrumentCode, other.currency);
    }
};

struct PositionAccumulator {
    std::string instrumentType;
    long long quantity = 0;
    long long cost = 0;
    long long realizedPnl = 0;
    long long totalFee = 0;
};

[[nodiscard]] std::string lowerCopy(std::string value)
{
    for (auto& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

[[nodiscard]] bool containsSensitiveToken(const std::string& value)
{
    const auto text = lowerCopy(value);
    for (const std::string token : {
             "credential",
             "password",
             "secret",
             "endpoint",
             "access_token",
             "broker_order",
             "broker order",
             "real order",
             "automatic trading",
         }) {
        if (text.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] bool hasSensitiveToken(const ShellAccountingReplayTradeFact& fact)
{
    return containsSensitiveToken(fact.factId) || containsSensitiveToken(fact.source) ||
        containsSensitiveToken(fact.note) || containsSensitiveToken(fact.instrumentCode);
}

[[nodiscard]] bool hasSensitiveToken(const ShellAccountingReplayCashFact& fact)
{
    return containsSensitiveToken(fact.factId) || containsSensitiveToken(fact.note) ||
        containsSensitiveToken(fact.action);
}

[[nodiscard]] AccountingIssueDto issue(
    std::string code,
    std::string message,
    std::string field,
    std::string sourceId = {})
{
    return makeAccountingIssue(
        AccountingIssueLevel::Error,
        std::move(code),
        std::move(message),
        true,
        std::move(field),
        std::move(sourceId));
}

[[nodiscard]] DecimalParseResult parseScaledDecimal(const std::string& text, bool allowNegative)
{
    if (text.empty()) {
        return {};
    }

    std::size_t index = 0;
    bool negative = false;
    if (text[index] == '-') {
        if (!allowNegative) {
            return {};
        }
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

    const auto value = units * kScale + fraction;
    return {true, negative ? -value : value};
}

[[nodiscard]] long long multiplyScaled(long long lhs, long long rhs)
{
    return static_cast<long long>(
        std::llround((static_cast<long double>(lhs) * static_cast<long double>(rhs)) /
                     static_cast<long double>(kScale)));
}

[[nodiscard]] std::string formatScaled(long long value, int decimals, bool trimZeros)
{
    const bool negative = value < 0;
    auto absolute = negative ? -value : value;
    const auto units = absolute / kScale;
    auto fraction = absolute % kScale;

    std::string fractionText = std::to_string(fraction + kScale).substr(1);
    if (decimals < 6) {
        const int shift = 6 - decimals;
        long long rounder = 1;
        for (int i = 0; i < shift; ++i) {
            rounder *= 10;
        }
        const auto rounded = (absolute + rounder / 2) / rounder;
        absolute = rounded * rounder;
        const auto roundedUnits = absolute / kScale;
        fraction = absolute % kScale;
        fractionText = std::to_string(fraction + kScale).substr(1);
        std::ostringstream roundedStream;
        if (negative) {
            roundedStream << '-';
        }
        roundedStream << roundedUnits;
        auto visibleFraction = fractionText.substr(0, static_cast<std::size_t>(decimals));
        if (trimZeros) {
            while (!visibleFraction.empty() && visibleFraction.back() == '0') {
                visibleFraction.pop_back();
            }
        }
        if (!visibleFraction.empty()) {
            roundedStream << '.' << visibleFraction;
        }
        return roundedStream.str();
    }

    if (trimZeros) {
        while (!fractionText.empty() && fractionText.back() == '0') {
            fractionText.pop_back();
        }
    }

    std::ostringstream stream;
    if (negative) {
        stream << '-';
    }
    stream << units;
    if (!fractionText.empty()) {
        stream << '.' << fractionText;
    }
    return stream.str();
}

[[nodiscard]] std::string formatMoney(long long value)
{
    return formatScaled(value, 2, false);
}

[[nodiscard]] std::string formatQuantity(long long value)
{
    return formatScaled(value, 6, true);
}

[[nodiscard]] std::string formatPrice(long long value)
{
    return formatScaled(value, 4, false);
}

[[nodiscard]] std::string normalizedInstrumentType(const std::string& value)
{
    const auto upper = lowerCopy(value);
    if (upper == "otc" || upper == "otc_fund") {
        return "OTC";
    }
    if (upper == "etf") {
        return "ETF";
    }
    return value.empty() ? "ETF" : value;
}

[[nodiscard]] std::string positionBucket(const std::string& instrumentType)
{
    return lowerCopy(instrumentType) == "otc" ? "OTC" : "ETF";
}

void appendIssue(std::vector<AccountingIssueDto>& issues, AccountingIssueDto next)
{
    issues.push_back(std::move(next));
}

template <typename T>
void sortFacts(std::vector<T>& facts)
{
    std::sort(facts.begin(), facts.end(), [](const T& lhs, const T& rhs) {
        const auto lhsTime = [&]() -> std::string {
            if constexpr (std::is_same_v<T, ShellAccountingReplayTradeFact>) {
                return lhs.tradeTime;
            } else {
                return lhs.time;
            }
        }();
        const auto rhsTime = [&]() -> std::string {
            if constexpr (std::is_same_v<T, ShellAccountingReplayTradeFact>) {
                return rhs.tradeTime;
            } else {
                return rhs.time;
            }
        }();
        return std::tie(lhsTime, lhs.factId) < std::tie(rhsTime, rhs.factId);
    });
}

[[nodiscard]] ShellAccountingReplayResult rejected(std::vector<AccountingIssueDto> issues)
{
    ShellAccountingReplayResult result;
    result.accepted = false;
    result.replayExecuted = true;
    result.readOnly = true;
    result.status = ShellAccountingReplayStatus::Rejected;
    result.issues = std::move(issues);
    result.pnlSummary.dataQualityStatus = "ERROR";
    result.pnlSummary.realizedPnlText = "0.00";
    result.pnlSummary.unrealizedPnlText = "0.00";
    result.pnlSummary.totalFeeText = "0.00";
    result.pnlSummary.totalMarketValueText = "0.00";
    return result;
}

} // namespace

ShellAccountingReplayResult ShellAccountingPortfolioReplayEngine::replayReadOnly(
    const ShellAccountingReplayInput& input) const
{
    std::vector<AccountingIssueDto> issues;
    std::set<std::string> factIds;

    for (const auto& cashFact : input.cashFacts) {
        if (cashFact.factId.empty()) {
            appendIssue(issues, issue("MISSING_FACT_ID", "Cash fact id is required.", "factId"));
        } else if (!factIds.insert(cashFact.factId).second) {
            appendIssue(issues, issue("DUPLICATE_FACT_ID", "Duplicate fact id rejected.", "factId", cashFact.factId));
        }
        if (hasSensitiveToken(cashFact)) {
            appendIssue(issues, issue("SENSITIVE_TOKEN_FORBIDDEN", "Sensitive token rejected.", "cashFacts", cashFact.factId));
        }
    }

    for (const auto& tradeFact : input.tradeFacts) {
        if (tradeFact.factId.empty()) {
            appendIssue(issues, issue("MISSING_FACT_ID", "Trade fact id is required.", "factId"));
        } else if (!factIds.insert(tradeFact.factId).second) {
            appendIssue(issues, issue("DUPLICATE_FACT_ID", "Duplicate fact id rejected.", "factId", tradeFact.factId));
        }
        if (tradeFact.instrumentCode.empty()) {
            appendIssue(issues, issue("MISSING_INSTRUMENT", "Instrument code is required.", "instrumentCode", tradeFact.factId));
        }
        if (hasSensitiveToken(tradeFact)) {
            appendIssue(issues, issue("BROKER_OR_SENSITIVE_TOKEN_FORBIDDEN", "Broker or sensitive token rejected.", "tradeFacts", tradeFact.factId));
        }
    }

    if (!issues.empty()) {
        return rejected(std::move(issues));
    }

    std::vector<ShellAccountingReplayCashFact> cashFacts = input.cashFacts;
    std::vector<ShellAccountingReplayTradeFact> tradeFacts = input.tradeFacts;
    sortFacts(cashFacts);
    sortFacts(tradeFacts);

    std::map<AccountPortfolioCurrencyKey, long long> cashBalances;
    std::map<PositionKey, PositionAccumulator> positions;
    std::string baseCurrency;

    for (const auto& cashFact : cashFacts) {
        if (cashFact.accountId.empty() || cashFact.portfolioId.empty() || cashFact.currency.empty()) {
            appendIssue(issues, issue("MISSING_CASH_SCOPE", "Cash fact account, portfolio, and currency are required.", "cashFacts", cashFact.factId));
            continue;
        }
        if (baseCurrency.empty()) {
            baseCurrency = cashFact.currency;
        } else if (baseCurrency != cashFact.currency) {
            appendIssue(issues, issue("CURRENCY_MISMATCH", "Multi-currency replay is not supported in EPIC-276.", "currency", cashFact.factId));
            continue;
        }

        const auto amount = parseScaledDecimal(cashFact.amountText, false);
        if (!amount.valid) {
            appendIssue(issues, issue("INVALID_CASH_AMOUNT", "Cash amount must be a non-negative decimal.", "amountText", cashFact.factId));
            continue;
        }

        const AccountPortfolioCurrencyKey key{cashFact.accountId, cashFact.portfolioId, cashFact.currency};
        if (cashFact.action == "INITIAL_CASH" || cashFact.action == "DEPOSIT" ||
            cashFact.action == "CASH_IN" || cashFact.action == "ADJUSTMENT") {
            cashBalances[key] += amount.value;
        } else if (cashFact.action == "WITHDRAW" || cashFact.action == "WITHDRAWAL" ||
                   cashFact.action == "CASH_OUT") {
            cashBalances[key] -= amount.value;
        } else {
            appendIssue(issues, issue("UNSUPPORTED_CASH_ACTION", "Unsupported cash action rejected.", "action", cashFact.factId));
            continue;
        }

        if (cashBalances[key] < 0) {
            appendIssue(issues, issue("NEGATIVE_CASH", "Cash audit failed closed.", "cashFacts", cashFact.factId));
        }
    }

    for (const auto& tradeFact : tradeFacts) {
        if (tradeFact.accountId.empty() || tradeFact.portfolioId.empty() || tradeFact.currency.empty()) {
            appendIssue(issues, issue("MISSING_TRADE_SCOPE", "Trade fact account, portfolio, and currency are required.", "tradeFacts", tradeFact.factId));
            continue;
        }
        if (baseCurrency.empty()) {
            baseCurrency = tradeFact.currency;
        } else if (baseCurrency != tradeFact.currency) {
            appendIssue(issues, issue("CURRENCY_MISMATCH", "Trade currency mismatches replay base currency.", "currency", tradeFact.factId));
            continue;
        }

        const auto quantity = parseScaledDecimal(tradeFact.quantityText, false);
        const auto price = parseScaledDecimal(tradeFact.priceText, false);
        const auto amountFromText = parseScaledDecimal(tradeFact.amountText, false);
        const auto fee = parseScaledDecimal(tradeFact.feeText.empty() ? "0" : tradeFact.feeText, false);
        const auto cashFlow = tradeFact.cashFlowText.empty()
            ? DecimalParseResult{false, 0}
            : parseScaledDecimal(tradeFact.cashFlowText, true);

        if (!quantity.valid || quantity.value <= 0) {
            appendIssue(issues, issue("INVALID_QUANTITY", "Trade quantity must be greater than zero.", "quantityText", tradeFact.factId));
            continue;
        }
        if (!price.valid || price.value <= 0) {
            appendIssue(issues, issue("INVALID_PRICE", "Trade price must be greater than zero.", "priceText", tradeFact.factId));
            continue;
        }
        if (!fee.valid) {
            appendIssue(issues, issue("INVALID_FEE", "Trade fee must be a non-negative decimal.", "feeText", tradeFact.factId));
            continue;
        }
        if (!tradeFact.cashFlowText.empty() && !cashFlow.valid) {
            appendIssue(issues, issue("INVALID_CASH_FLOW", "Trade cash flow must be a decimal.", "cashFlowText", tradeFact.factId));
            continue;
        }

        const auto computedAmount = multiplyScaled(quantity.value, price.value);
        const auto amount = amountFromText.valid ? amountFromText.value : computedAmount;
        const AccountPortfolioCurrencyKey cashKey{
            tradeFact.accountId,
            tradeFact.portfolioId,
            tradeFact.currency,
        };
        const PositionKey positionKey{
            tradeFact.accountId,
            tradeFact.portfolioId,
            tradeFact.instrumentCode,
            tradeFact.currency,
        };
        auto& position = positions[positionKey];
        if (position.instrumentType.empty()) {
            position.instrumentType = normalizedInstrumentType(tradeFact.instrumentType);
        }

        if (tradeFact.side == "BUY") {
            const auto cost = amount + fee.value;
            position.quantity += quantity.value;
            position.cost += cost;
            position.totalFee += fee.value;
            cashBalances[cashKey] += cashFlow.valid ? cashFlow.value : -cost;
        } else if (tradeFact.side == "SELL") {
            if (position.quantity < quantity.value) {
                appendIssue(issues, issue("SELL_OVER_POSITION", "Sell quantity exceeds available position.", "quantityText", tradeFact.factId));
                continue;
            }
            const auto allocatedCost = static_cast<long long>(
                std::llround((static_cast<long double>(position.cost) *
                              static_cast<long double>(quantity.value)) /
                             static_cast<long double>(position.quantity)));
            const auto proceedsAfterFee = amount - fee.value;
            position.quantity -= quantity.value;
            position.cost -= allocatedCost;
            position.realizedPnl += proceedsAfterFee - allocatedCost;
            position.totalFee += fee.value;
            cashBalances[cashKey] += cashFlow.valid ? cashFlow.value : proceedsAfterFee;
            if (position.quantity == 0) {
                position.cost = 0;
            }
        } else {
            appendIssue(issues, issue("UNSUPPORTED_SIDE", "Unsupported trade side rejected.", "side", tradeFact.factId));
            continue;
        }

        if (cashBalances[cashKey] < 0) {
            appendIssue(issues, issue("NEGATIVE_CASH", "Cash audit failed closed after trade replay.", "cashFacts", tradeFact.factId));
        }
    }

    if (!issues.empty()) {
        return rejected(std::move(issues));
    }

    std::map<std::pair<std::string, std::string>, long long> marketPrices;
    for (const auto& priceFact : input.marketPriceFacts) {
        const auto price = parseScaledDecimal(priceFact.priceText, false);
        if (!price.valid || price.value <= 0 || priceFact.instrumentCode.empty() || priceFact.currency.empty()) {
            appendIssue(issues, issue("INVALID_MARKET_PRICE", "Invalid market price fact rejected.", "marketPriceFacts", priceFact.instrumentCode));
            continue;
        }
        marketPrices[{priceFact.instrumentCode, priceFact.currency}] = price.value;
    }
    if (!issues.empty()) {
        return rejected(std::move(issues));
    }

    ShellAccountingReplayResult result;
    result.accepted = true;
    result.replayExecuted = true;
    result.readOnly = true;
    result.status = ShellAccountingReplayStatus::Completed;
    result.pnlSummary.currency = baseCurrency.empty() ? "CNY" : baseCurrency;
    result.pnlSummary.dataQualityStatus = "OK";

    long long realizedPnl = 0;
    long long unrealizedPnl = 0;
    long long totalFee = 0;
    long long totalMarketValue = 0;
    bool missingMarketPrice = false;

    for (const auto& [key, position] : positions) {
        realizedPnl += position.realizedPnl;
        totalFee += position.totalFee;
        if (position.quantity == 0) {
            continue;
        }
        const auto averageCost = static_cast<long long>(
            std::llround((static_cast<long double>(position.cost) * static_cast<long double>(kScale)) /
                         static_cast<long double>(position.quantity)));
        long long marketValue = 0;
        long long positionUnrealized = 0;
        std::string marketValueText = "UNAVAILABLE";
        std::string unrealizedText = "UNAVAILABLE";
        std::string dataQuality = "PARTIAL";
        const auto priceIt = marketPrices.find({key.instrumentCode, key.currency});
        if (priceIt != marketPrices.end()) {
            marketValue = multiplyScaled(position.quantity, priceIt->second);
            positionUnrealized = marketValue - position.cost;
            marketValueText = formatMoney(marketValue);
            unrealizedText = formatMoney(positionUnrealized);
            totalMarketValue += marketValue;
            unrealizedPnl += positionUnrealized;
            dataQuality = "OK";
        } else {
            missingMarketPrice = true;
        }

        result.positions.push_back(ShellAccountingReplayPosition{
            key.accountId,
            key.portfolioId,
            key.instrumentCode,
            position.instrumentType.empty() ? "ETF" : position.instrumentType,
            positionBucket(position.instrumentType),
            formatQuantity(position.quantity),
            formatPrice(averageCost),
            formatMoney(position.cost),
            marketValueText,
            formatMoney(position.realizedPnl),
            unrealizedText,
            formatMoney(position.totalFee),
            key.currency,
            dataQuality,
        });
    }

    for (const auto& [key, balance] : cashBalances) {
        result.cashSummaries.push_back(ShellAccountingReplayCashSummary{
            key.accountId,
            key.portfolioId,
            key.currency,
            formatMoney(balance),
            "OK",
        });
    }

    std::sort(result.positions.begin(), result.positions.end(), [](const auto& lhs, const auto& rhs) {
        return std::tie(lhs.accountId, lhs.portfolioId, lhs.instrumentCode, lhs.currency) <
            std::tie(rhs.accountId, rhs.portfolioId, rhs.instrumentCode, rhs.currency);
    });
    std::sort(result.cashSummaries.begin(), result.cashSummaries.end(), [](const auto& lhs, const auto& rhs) {
        return std::tie(lhs.accountId, lhs.portfolioId, lhs.currency) <
            std::tie(rhs.accountId, rhs.portfolioId, rhs.currency);
    });

    result.pnlSummary.realizedPnlText = formatMoney(realizedPnl);
    result.pnlSummary.unrealizedPnlText = missingMarketPrice ? "UNAVAILABLE" : formatMoney(unrealizedPnl);
    result.pnlSummary.totalFeeText = formatMoney(totalFee);
    result.pnlSummary.totalMarketValueText = missingMarketPrice ? "UNAVAILABLE" : formatMoney(totalMarketValue);
    result.pnlSummary.dataQualityStatus = missingMarketPrice ? "PARTIAL" : "OK";
    return result;
}

ShellAccountingReplayResult replayShellAccountingPortfolioReadOnly(
    const ShellAccountingReplayInput& input)
{
    return ShellAccountingPortfolioReplayEngine{}.replayReadOnly(input);
}

} // namespace etfdt::accounting
