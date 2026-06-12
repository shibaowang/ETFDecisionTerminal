#include "StrategyEngine/StrategyRecommendationEngine.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace etfdt::strategy_engine {
namespace {

struct JsonValue final {
    enum class Type {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object,
    };

    Type type = Type::Null;
    bool boolValue = false;
    double numberValue = 0.0;
    std::string stringValue;
    std::vector<JsonValue> arrayValue;
    std::map<std::string, JsonValue> objectValue;
};

class JsonParser final {
public:
    explicit JsonParser(std::string_view text)
        : text_(text)
    {
    }

    [[nodiscard]] std::optional<JsonValue> parse()
    {
        auto value = parseValue();
        skipWhitespace();
        if (!value.has_value() || position_ != text_.size()) {
            return std::nullopt;
        }
        return value;
    }

private:
    void skipWhitespace()
    {
        while (position_ < text_.size()
               && std::isspace(static_cast<unsigned char>(text_[position_])) != 0) {
            ++position_;
        }
    }

    [[nodiscard]] bool consume(char expected)
    {
        skipWhitespace();
        if (position_ >= text_.size() || text_[position_] != expected) {
            return false;
        }
        ++position_;
        return true;
    }

    [[nodiscard]] std::optional<JsonValue> parseValue()
    {
        skipWhitespace();
        if (position_ >= text_.size()) {
            return std::nullopt;
        }

        const char ch = text_[position_];
        if (ch == '"') {
            auto parsed = parseString();
            if (!parsed.has_value()) {
                return std::nullopt;
            }
            JsonValue value;
            value.type = JsonValue::Type::String;
            value.stringValue = std::move(*parsed);
            return value;
        }
        if (ch == '{') {
            return parseObject();
        }
        if (ch == '[') {
            return parseArray();
        }
        if (startsWith("true")) {
            position_ += 4U;
            JsonValue value;
            value.type = JsonValue::Type::Bool;
            value.boolValue = true;
            return value;
        }
        if (startsWith("false")) {
            position_ += 5U;
            JsonValue value;
            value.type = JsonValue::Type::Bool;
            return value;
        }
        if (startsWith("null")) {
            position_ += 4U;
            return JsonValue {};
        }
        if (ch == '-' || std::isdigit(static_cast<unsigned char>(ch)) != 0) {
            return parseNumber();
        }
        return std::nullopt;
    }

    [[nodiscard]] bool startsWith(std::string_view token) const
    {
        return text_.substr(position_, token.size()) == token;
    }

    [[nodiscard]] std::optional<std::string> parseString()
    {
        if (!consume('"')) {
            return std::nullopt;
        }

        std::string output;
        while (position_ < text_.size()) {
            const char ch = text_[position_++];
            if (ch == '"') {
                return output;
            }
            if (ch != '\\') {
                output.push_back(ch);
                continue;
            }
            if (position_ >= text_.size()) {
                return std::nullopt;
            }
            const char escaped = text_[position_++];
            switch (escaped) {
            case '"':
            case '\\':
            case '/':
                output.push_back(escaped);
                break;
            case 'b':
                output.push_back('\b');
                break;
            case 'f':
                output.push_back('\f');
                break;
            case 'n':
                output.push_back('\n');
                break;
            case 'r':
                output.push_back('\r');
                break;
            case 't':
                output.push_back('\t');
                break;
            case 'u':
                if (position_ + 4U > text_.size()) {
                    return std::nullopt;
                }
                position_ += 4U;
                output.push_back('?');
                break;
            default:
                return std::nullopt;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<JsonValue> parseNumber()
    {
        const auto start = position_;
        if (text_[position_] == '-') {
            ++position_;
        }
        while (position_ < text_.size()
               && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
            ++position_;
        }
        if (position_ < text_.size() && text_[position_] == '.') {
            ++position_;
            while (position_ < text_.size()
                   && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
                ++position_;
            }
        }
        if (position_ < text_.size() && (text_[position_] == 'e' || text_[position_] == 'E')) {
            ++position_;
            if (position_ < text_.size() && (text_[position_] == '+' || text_[position_] == '-')) {
                ++position_;
            }
            while (position_ < text_.size()
                   && std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
                ++position_;
            }
        }

        try {
            JsonValue value;
            value.type = JsonValue::Type::Number;
            value.numberValue = std::stod(std::string(text_.substr(start, position_ - start)));
            return value;
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

    [[nodiscard]] std::optional<JsonValue> parseObject()
    {
        if (!consume('{')) {
            return std::nullopt;
        }

        JsonValue object;
        object.type = JsonValue::Type::Object;
        while (true) {
            skipWhitespace();
            if (consume('}')) {
                return object;
            }

            auto key = parseString();
            if (!key.has_value() || !consume(':')) {
                return std::nullopt;
            }
            auto value = parseValue();
            if (!value.has_value()) {
                return std::nullopt;
            }
            object.objectValue[*key] = std::move(*value);

            skipWhitespace();
            if (consume('}')) {
                return object;
            }
            if (!consume(',')) {
                return std::nullopt;
            }
        }
    }

    [[nodiscard]] std::optional<JsonValue> parseArray()
    {
        if (!consume('[')) {
            return std::nullopt;
        }

        JsonValue array;
        array.type = JsonValue::Type::Array;
        while (true) {
            skipWhitespace();
            if (consume(']')) {
                return array;
            }
            auto value = parseValue();
            if (!value.has_value()) {
                return std::nullopt;
            }
            array.arrayValue.push_back(std::move(*value));

            skipWhitespace();
            if (consume(']')) {
                return array;
            }
            if (!consume(',')) {
                return std::nullopt;
            }
        }
    }

    std::string_view text_;
    std::size_t position_ = 0;
};

[[nodiscard]] std::string trim(std::string value)
{
    auto begin = value.begin();
    while (begin != value.end() && std::isspace(static_cast<unsigned char>(*begin)) != 0) {
        ++begin;
    }
    auto end = value.end();
    while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1))) != 0) {
        --end;
    }
    return std::string(begin, end);
}

[[nodiscard]] std::string upper(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

[[nodiscard]] bool containsToken(std::string text, const std::vector<std::string>& tokens)
{
    text = upper(std::move(text));
    for (auto token : tokens) {
        token = upper(std::move(token));
        if (text.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::optional<double> parseDecimal(
    const std::string& value,
    bool allowEmpty = false)
{
    const auto trimmed = trim(value);
    if (trimmed.empty()) {
        return allowEmpty ? std::optional<double>(0.0) : std::nullopt;
    }
    try {
        std::size_t parsed = 0;
        const auto number = std::stod(trimmed, &parsed);
        if (parsed != trimmed.size() || !std::isfinite(number)) {
            return std::nullopt;
        }
        return number;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

[[nodiscard]] std::string formatFixed(double value, int decimals)
{
    if (std::fabs(value) < 0.0000001) {
        value = 0.0;
    }
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals) << value;
    return stream.str();
}

[[nodiscard]] std::string formatQuantity(double value, bool fourDecimals)
{
    if (!fourDecimals && std::fabs(value - std::round(value)) < 0.0000001) {
        return std::to_string(static_cast<long long>(std::llround(value)));
    }
    return formatFixed(value, fourDecimals ? 4 : 2);
}

[[nodiscard]] StrategyRecommendationIssue makeIssue(
    std::string code,
    std::string field,
    std::string message)
{
    StrategyRecommendationIssue issue;
    issue.code = std::move(code);
    issue.field = std::move(field);
    issue.message = std::move(message);
    return issue;
}

[[nodiscard]] StrategyRecommendationResult rejected(
    std::string code,
    std::string field,
    std::string message)
{
    StrategyRecommendationResult result;
    result.issues.push_back(makeIssue(std::move(code), std::move(field), std::move(message)));
    return result;
}

void acceptCommon(
    StrategyRecommendationResult& result,
    std::string actionCode,
    std::string actionLabel,
    std::string reasonCode,
    std::string reasonLabel,
    std::string sourceCode,
    std::string sourceLabel)
{
    result.accepted = true;
    result.recommendationComputed = true;
    result.status = "OK";
    result.actionCode = std::move(actionCode);
    result.actionLabel = std::move(actionLabel);
    result.reasonCode = std::move(reasonCode);
    result.reasonLabel = std::move(reasonLabel);
    result.sourceCode = std::move(sourceCode);
    result.sourceLabel = std::move(sourceLabel);
}

const JsonValue* objectField(const JsonValue& object, const std::string& key)
{
    if (object.type != JsonValue::Type::Object) {
        return nullptr;
    }
    const auto iterator = object.objectValue.find(key);
    return iterator == object.objectValue.end() ? nullptr : &iterator->second;
}

std::string jsonStringField(const JsonValue& object, const std::string& key)
{
    const auto* value = objectField(object, key);
    if (value == nullptr) {
        return {};
    }
    if (value->type == JsonValue::Type::String) {
        return value->stringValue;
    }
    if (value->type == JsonValue::Type::Number) {
        return formatFixed(value->numberValue, 6);
    }
    if (value->type == JsonValue::Type::Bool) {
        return value->boolValue ? "true" : "false";
    }
    return {};
}

int jsonIntField(const JsonValue& object, const std::string& key, int fallback)
{
    const auto* value = objectField(object, key);
    if (value == nullptr) {
        return fallback;
    }
    if (value->type == JsonValue::Type::Number) {
        return static_cast<int>(std::llround(value->numberValue));
    }
    if (value->type == JsonValue::Type::String) {
        const auto parsed = parseDecimal(value->stringValue);
        return parsed.has_value() ? static_cast<int>(std::llround(*parsed)) : fallback;
    }
    return fallback;
}

void decodeObject(const JsonValue& object, StrategyRecommendationInput& input)
{
    input.accountId = jsonStringField(object, "accountId");
    input.portfolioId = jsonStringField(object, "portfolioId");
    input.instrumentCode = jsonStringField(object, "instrumentCode");
    input.instrumentType = jsonStringField(object, "instrumentType");
    input.strategyCode = jsonStringField(object, "strategyCode");
    input.tradeSource = jsonStringField(object, "tradeSource");
    input.currentPriceText = jsonStringField(object, "currentPriceText");
    input.otcNavText = jsonStringField(object, "otcNavText");
    input.marketPriceText = jsonStringField(object, "marketPriceText");
    input.indexDrawdownText = jsonStringField(object, "indexDrawdownText");
    input.premiumText = jsonStringField(object, "premiumText");
    input.returnRateText = jsonStringField(object, "returnRateText");
    input.principalBaseText = jsonStringField(object, "principalBaseText");
    input.holdingCostText = jsonStringField(object, "holdingCostText");
    input.positionQuantityText = jsonStringField(object, "positionQuantityText");
    input.positionAverageCostText = jsonStringField(object, "positionAverageCostText");
    input.cashBalanceText = jsonStringField(object, "cashBalanceText");
    input.targetGridAmountText = jsonStringField(object, "targetGridAmountText");
    input.cumulativeTierRemainingText = jsonStringField(object, "cumulativeTierRemainingText");
    input.sellReturnThresholdText = jsonStringField(object, "sellReturnThresholdText");
    input.takeProfitPremiumThresholdText = jsonStringField(object, "takeProfitPremiumThresholdText");
    input.extremePremiumThresholdText = jsonStringField(object, "extremePremiumThresholdText");
    input.basePositionRatioText = jsonStringField(object, "basePositionRatioText");
    input.feeRateText = jsonStringField(object, "feeRateText");
    input.fixedFeeText = jsonStringField(object, "fixedFeeText");
    input.requestedSellQuantityText = jsonStringField(object, "requestedSellQuantityText");
    input.memo = jsonStringField(object, "memo");
    input.minLotSize = jsonIntField(object, "minLotSize", 100);

    input.buyThresholds.clear();
    const auto* thresholds = objectField(object, "buyThresholds");
    if (thresholds == nullptr || thresholds->type != JsonValue::Type::Array) {
        return;
    }
    for (const auto& value : thresholds->arrayValue) {
        if (value.type != JsonValue::Type::Object) {
            continue;
        }
        StrategyRecommendationBuyThreshold threshold;
        threshold.tierLabel = jsonStringField(value, "tierLabel");
        threshold.drawdownText = jsonStringField(value, "drawdownText");
        threshold.targetAmountText = jsonStringField(value, "targetAmountText");
        input.buyThresholds.push_back(std::move(threshold));
    }
}

std::string inputTokenText(const StrategyRecommendationInput& input)
{
    std::ostringstream stream;
    stream << input.accountId << ' ' << input.portfolioId << ' ' << input.instrumentCode << ' '
           << input.instrumentType << ' ' << input.strategyCode << ' ' << input.tradeSource << ' '
           << input.memo;
    return stream.str();
}

std::optional<StrategyRecommendationBuyThreshold> matchedBuyThreshold(
    const StrategyRecommendationInput& input,
    double indexDrawdown)
{
    std::optional<StrategyRecommendationBuyThreshold> matched;
    double matchedDrawdown = -1.0;
    for (const auto& threshold : input.buyThresholds) {
        const auto drawdown = parseDecimal(threshold.drawdownText);
        const auto target = parseDecimal(threshold.targetAmountText);
        if (!drawdown.has_value() || !target.has_value() || *drawdown <= 0.0 || *target <= 0.0
            || threshold.tierLabel.empty()) {
            return std::nullopt;
        }
        if (indexDrawdown + 0.0000001 >= *drawdown && *drawdown >= matchedDrawdown) {
            matched = threshold;
            matchedDrawdown = *drawdown;
        }
    }
    return matched;
}

double boundedTargetAmount(
    double thresholdTarget,
    double targetGridAmount,
    double cumulativeRemaining)
{
    double target = thresholdTarget;
    if (targetGridAmount > 0.0) {
        target = std::min(target, targetGridAmount);
    }
    if (cumulativeRemaining > 0.0) {
        target = std::min(target, cumulativeRemaining);
    }
    return target;
}

StrategyRecommendationResult makeWaitData(const StrategyRecommendationInput& input)
{
    StrategyRecommendationResult result;
    acceptCommon(
        result,
        "WAIT",
        "Wait",
        "WAIT_DATA",
        "Waiting for market data",
        input.tradeSource,
        input.tradeSource == "OTC_REPLACEMENT" ? "OTC replacement" : "ETF direct");
    result.targetAmountText = "0.00";
    result.suggestedQuantityText = "0";
    result.suggestedAmountText = "0.00";
    result.netCashImpactText = "0.00";
    result.feeText = "0.00";
    result.issues.push_back(makeIssue(
        "WAIT_MISSING_MARKET_DATA",
        "currentPriceText",
        "Market price is unavailable; recommendation remains read-only."));
    return result;
}

}  // namespace

StrategyRecommendationResult StrategyRecommendationEngine::recommendReadOnly(
    const StrategyRecommendationInput& input) const
{
    const auto tradeSource = upper(trim(input.tradeSource.empty() ? "ETF_DIRECT" : input.tradeSource));
    if (trim(input.instrumentCode).empty()) {
        return rejected("MISSING_INSTRUMENT", "instrumentCode", "Instrument code is required.");
    }
    if (containsToken(inputTokenText(input), {"password", "credential", "secret", "endpoint"})) {
        return rejected("SENSITIVE_TOKEN", "payload", "Sensitive token is not accepted.");
    }
    if (containsToken(inputTokenText(input), {"broker_order", "brokerorder", "order.submit", "real_order"})) {
        return rejected("BROKER_ORDER_TOKEN", "payload", "Broker order token is not accepted.");
    }
    if (containsToken(
            inputTokenText(input),
            {"handsfree_execution", "hands-free-execution", "unsupervised_execution"})) {
        return rejected("AUTOMATIC_TRADING_TOKEN", "payload", "Automatic trading token is not accepted.");
    }
    if (tradeSource != "ETF_DIRECT" && tradeSource != "OTC_REPLACEMENT") {
        return rejected("UNSUPPORTED_TRADE_SOURCE", "tradeSource", "Unsupported trade source.");
    }

    const auto currentPrice = parseDecimal(input.currentPriceText, true);
    if (!currentPrice.has_value() || *currentPrice < 0.0) {
        return rejected("INVALID_PRICE", "currentPriceText", "Current price must be a non-negative number.");
    }
    if (*currentPrice == 0.0 && tradeSource == "ETF_DIRECT") {
        return makeWaitData(input);
    }
    const auto otcNav = parseDecimal(input.otcNavText, true);
    if (tradeSource == "OTC_REPLACEMENT" && (!otcNav.has_value() || *otcNav <= 0.0)) {
        return rejected("INVALID_PRICE", "otcNavText", "OTC NAV must be greater than zero.");
    }

    const auto cashBalance = parseDecimal(input.cashBalanceText);
    if (!cashBalance.has_value() || *cashBalance < 0.0) {
        return rejected("INVALID_CASH", "cashBalanceText", "Cash balance must be non-negative.");
    }
    const auto positionQuantity = parseDecimal(input.positionQuantityText, true);
    if (!positionQuantity.has_value() || *positionQuantity < 0.0) {
        return rejected("INVALID_POSITION_QUANTITY", "positionQuantityText", "Position quantity must be non-negative.");
    }
    const auto principalBase = parseDecimal(input.principalBaseText);
    if (!principalBase.has_value() || *principalBase < 0.0) {
        return rejected("INVALID_PRINCIPAL_BASE", "principalBaseText", "Principal base must be non-negative.");
    }
    const auto positionAverageCost = parseDecimal(input.positionAverageCostText, true);
    if (!positionAverageCost.has_value() || *positionAverageCost < 0.0) {
        return rejected("INVALID_POSITION_AVERAGE_COST", "positionAverageCostText", "Position average cost must be non-negative.");
    }
    const auto indexDrawdown = parseDecimal(input.indexDrawdownText, true).value_or(0.0);
    const auto premium = parseDecimal(input.premiumText, true).value_or(0.0);
    const auto returnRate = parseDecimal(input.returnRateText, true).value_or(0.0);
    const auto targetGridAmount = parseDecimal(input.targetGridAmountText, true).value_or(0.0);
    const auto cumulativeRemaining = parseDecimal(input.cumulativeTierRemainingText, true).value_or(0.0);
    const auto sellReturnThreshold = parseDecimal(input.sellReturnThresholdText, true).value_or(0.0);
    const auto takeProfitPremiumThreshold = parseDecimal(input.takeProfitPremiumThresholdText, true).value_or(0.0);
    const auto extremePremiumThreshold = parseDecimal(input.extremePremiumThresholdText, true).value_or(0.0);
    const auto basePositionRatio = parseDecimal(input.basePositionRatioText, true).value_or(0.20);
    const auto feeRate = parseDecimal(input.feeRateText, true).value_or(0.0);
    const auto fixedFee = parseDecimal(input.fixedFeeText, true).value_or(0.0);
    if (basePositionRatio < 0.0 || basePositionRatio > 1.0 || input.minLotSize <= 0) {
        return rejected("INVALID_THRESHOLDS", "basePositionRatioText", "Base position ratio and lot size are invalid.");
    }

    const auto requestedSellQuantity = parseDecimal(input.requestedSellQuantityText, true).value_or(0.0);
    if (requestedSellQuantity > *positionQuantity + 0.0000001) {
        return rejected("SELL_OVER_POSITION", "requestedSellQuantityText", "Requested sell quantity exceeds position.");
    }

    std::string sellReason;
    std::string sellReasonLabel;
    if (extremePremiumThreshold > 0.0 && premium >= extremePremiumThreshold) {
        sellReason = "EXTREME_PREMIUM";
        sellReasonLabel = "Extreme premium";
    } else if (takeProfitPremiumThreshold > 0.0 && premium >= takeProfitPremiumThreshold) {
        sellReason = "TAKE_PROFIT";
        sellReasonLabel = "Take profit premium";
    } else if (sellReturnThreshold > 0.0 && returnRate >= sellReturnThreshold) {
        sellReason = "SELL_RETURN";
        sellReasonLabel = "Return threshold reached";
    }

    const bool hasPosition = *positionQuantity > 0.0000001;
    if (!sellReason.empty() && hasPosition) {
        const double price = tradeSource == "OTC_REPLACEMENT" ? *otcNav : *currentPrice;
        const double averageCost = *positionAverageCost > 0.0 ? *positionAverageCost : price;
        const double currentCostBasis = parseDecimal(input.holdingCostText, true).value_or(0.0) > 0.0
            ? parseDecimal(input.holdingCostText, true).value_or(0.0)
            : *positionQuantity * averageCost;
        const double baseRequired = *principalBase * basePositionRatio;
        const double maxSellByBase = averageCost <= 0.0
            ? 0.0
            : std::max(0.0, (currentCostBasis - baseRequired + 0.01) / averageCost);
        double sellQuantity = std::min(*positionQuantity, maxSellByBase);
        if (requestedSellQuantity > 0.0) {
            sellQuantity = std::min(sellQuantity, requestedSellQuantity);
        }
        if (tradeSource == "ETF_DIRECT") {
            sellQuantity = std::floor(sellQuantity / input.minLotSize) * input.minLotSize;
        }
        if (sellQuantity <= 0.0000001) {
            StrategyRecommendationResult result;
            acceptCommon(
                result,
                "HOLD",
                "Hold",
                "BASE_PROTECTED",
                "Base position protected",
                tradeSource,
                tradeSource == "OTC_REPLACEMENT" ? "OTC replacement" : "ETF direct");
            result.baseProtectionPassed = false;
            result.targetAmountText = "0.00";
            result.suggestedQuantityText = "0";
            result.suggestedAmountText = "0.00";
            result.netCashImpactText = "0.00";
            result.feeText = "0.00";
            return result;
        }

        const double gross = sellQuantity * price;
        const double fee = gross * feeRate + fixedFee;
        StrategyRecommendationResult result;
        acceptCommon(
            result,
            "SELL",
            "Sell",
            sellReason,
            sellReasonLabel,
            tradeSource,
            tradeSource == "OTC_REPLACEMENT" ? "OTC replacement" : "ETF direct");
        result.targetAmountText = formatFixed(gross, 2);
        result.suggestedQuantityText = formatQuantity(sellQuantity, tradeSource == "OTC_REPLACEMENT");
        result.suggestedAmountText = formatFixed(gross, 2);
        result.netCashImpactText = formatFixed(gross - fee, 2);
        result.feeText = formatFixed(fee, 2);
        result.baseProtectionPassed = true;
        return result;
    }

    if (input.buyThresholds.empty()) {
        return rejected("INVALID_THRESHOLDS", "buyThresholds", "At least one buy threshold is required.");
    }

    bool invalidThreshold = false;
    for (const auto& threshold : input.buyThresholds) {
        const auto drawdown = parseDecimal(threshold.drawdownText);
        const auto target = parseDecimal(threshold.targetAmountText);
        invalidThreshold = invalidThreshold || !drawdown.has_value() || !target.has_value()
            || *drawdown <= 0.0 || *target <= 0.0 || threshold.tierLabel.empty();
    }
    if (invalidThreshold) {
        return rejected("INVALID_THRESHOLDS", "buyThresholds", "Buy thresholds are invalid.");
    }

    const auto threshold = matchedBuyThreshold(input, indexDrawdown);
    if (threshold.has_value()) {
        const double thresholdTarget = parseDecimal(threshold->targetAmountText).value_or(0.0);
        const double targetAmount = boundedTargetAmount(thresholdTarget, targetGridAmount, cumulativeRemaining);
        const double price = tradeSource == "OTC_REPLACEMENT" ? *otcNav : *currentPrice;
        const double maxSpend = std::min(targetAmount, *cashBalance);
        if (maxSpend <= 0.0) {
            return rejected("NO_CASH_FOR_MIN_LOT", "cashBalanceText", "Cash cannot satisfy the minimum buy amount.");
        }

        StrategyRecommendationResult result;
        acceptCommon(
            result,
            "BUY",
            "Buy",
            "BUY_GRID_TIER",
            "Buy grid tier reached",
            tradeSource == "OTC_REPLACEMENT" ? "OTC_REPLACEMENT_GENERIC" : "ETF_DIRECT",
            tradeSource == "OTC_REPLACEMENT" ? "OTC replacement generic" : "ETF direct");
        result.tierLabel = threshold->tierLabel;
        result.targetAmountText = formatFixed(targetAmount, 2);
        result.cashLimitApplied = *cashBalance + 0.0000001 < targetAmount;

        if (tradeSource == "OTC_REPLACEMENT") {
            const double amount = std::floor(maxSpend * 100.0) / 100.0;
            if (amount < 0.01) {
                return rejected("NO_CASH_FOR_MIN_LOT", "cashBalanceText", "Cash cannot satisfy the minimum OTC amount.");
            }
            const double quantity = amount / price;
            const double fee = amount * feeRate + fixedFee;
            result.suggestedQuantityText = formatQuantity(quantity, true);
            result.suggestedAmountText = formatFixed(amount, 2);
            result.netCashImpactText = formatFixed(-(amount + fee), 2);
            result.feeText = formatFixed(fee, 2);
            return result;
        }

        const double quantity =
            std::floor(maxSpend / price / input.minLotSize) * input.minLotSize;
        if (quantity < input.minLotSize) {
            return rejected("NO_CASH_FOR_MIN_LOT", "cashBalanceText", "Cash cannot satisfy the minimum ETF lot.");
        }
        const double amount = quantity * price;
        const double fee = amount * feeRate + fixedFee;
        result.suggestedQuantityText = formatQuantity(quantity, false);
        result.suggestedAmountText = formatFixed(amount, 2);
        result.netCashImpactText = formatFixed(-(amount + fee), 2);
        result.feeText = formatFixed(fee, 2);
        return result;
    }

    StrategyRecommendationResult result;
    const bool waitBuild = !hasPosition;
    acceptCommon(
        result,
        waitBuild ? "WAIT" : "HOLD",
        waitBuild ? "Wait" : "Hold",
        waitBuild ? "WAIT_BUILD_POSITION" : "NO_SIGNAL",
        waitBuild ? "Waiting to build position" : "No signal",
        tradeSource,
        tradeSource == "OTC_REPLACEMENT" ? "OTC replacement" : "ETF direct");
    result.targetAmountText = "0.00";
    result.suggestedQuantityText = "0";
    result.suggestedAmountText = "0.00";
    result.netCashImpactText = "0.00";
    result.feeText = "0.00";
    return result;
}

bool decodeStrategyRecommendationInputJson(
    const std::string& payloadJson,
    StrategyRecommendationInput& input,
    std::string& errorCode)
{
    const auto parsed = JsonParser(payloadJson).parse();
    if (!parsed.has_value() || parsed->type != JsonValue::Type::Object) {
        errorCode = "STRATEGY_RECOMMENDATION_PAYLOAD_PARSE_ERROR";
        return false;
    }

    const JsonValue* inputObject = objectField(*parsed, "input");
    if (inputObject == nullptr) {
        inputObject = &*parsed;
    }
    if (inputObject->type != JsonValue::Type::Object) {
        errorCode = "STRATEGY_RECOMMENDATION_INPUT_PARSE_ERROR";
        return false;
    }

    decodeObject(*inputObject, input);
    return true;
}

}  // namespace etfdt::strategy_engine
