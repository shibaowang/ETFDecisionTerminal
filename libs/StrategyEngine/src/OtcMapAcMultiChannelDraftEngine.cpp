#include "StrategyEngine/OtcMapAcMultiChannelDraftEngine.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string_view>
#include <utility>

namespace etfdt::strategy_engine {
namespace {

struct JsonValue final {
    enum class Type { Null, Bool, Number, String, Array, Object };
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

    std::optional<JsonValue> parse()
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

    bool consume(char expected)
    {
        skipWhitespace();
        if (position_ >= text_.size() || text_[position_] != expected) {
            return false;
        }
        ++position_;
        return true;
    }

    std::optional<JsonValue> parseValue()
    {
        skipWhitespace();
        if (position_ >= text_.size()) {
            return std::nullopt;
        }
        if (text_[position_] == '"') {
            auto parsed = parseString();
            if (!parsed.has_value()) {
                return std::nullopt;
            }
            JsonValue value;
            value.type = JsonValue::Type::String;
            value.stringValue = std::move(*parsed);
            return value;
        }
        if (text_[position_] == '{') {
            return parseObject();
        }
        if (text_[position_] == '[') {
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
        if (text_[position_] == '-' || std::isdigit(static_cast<unsigned char>(text_[position_])) != 0) {
            return parseNumber();
        }
        return std::nullopt;
    }

    bool startsWith(std::string_view token) const
    {
        return text_.substr(position_, token.size()) == token;
    }

    std::optional<std::string> parseString()
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

    std::optional<JsonValue> parseNumber()
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
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<JsonValue> parseObject()
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

    std::optional<JsonValue> parseArray()
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

std::string trim(std::string value)
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

std::string upper(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return value;
}

bool blank(const std::string& value)
{
    return trim(value).empty();
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
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(6) << value->numberValue;
        auto text = stream.str();
        while (!text.empty() && text.back() == '0') {
            text.pop_back();
        }
        if (!text.empty() && text.back() == '.') {
            text.pop_back();
        }
        return text;
    }
    if (value->type == JsonValue::Type::Bool) {
        return value->boolValue ? "true" : "false";
    }
    return {};
}

bool jsonBoolField(const JsonValue& object, const std::string& key, bool fallback)
{
    const auto* value = objectField(object, key);
    if (value == nullptr) {
        return fallback;
    }
    if (value->type == JsonValue::Type::Bool) {
        return value->boolValue;
    }
    if (value->type == JsonValue::Type::String) {
        return upper(trim(value->stringValue)) == "TRUE";
    }
    return fallback;
}

int jsonIntField(const JsonValue& object, const std::string& key, int fallback)
{
    const auto text = jsonStringField(object, key);
    if (blank(text)) {
        return fallback;
    }
    try {
        std::size_t parsed = 0;
        const auto value = std::stoi(text, &parsed, 10);
        return parsed == trim(text).size() ? value : fallback;
    } catch (...) {
        return fallback;
    }
}

std::int64_t jsonInt64Field(const JsonValue& object, const std::string& key, std::int64_t fallback)
{
    const auto text = jsonStringField(object, key);
    if (blank(text)) {
        return fallback;
    }
    try {
        std::size_t parsed = 0;
        const auto value = std::stoll(text, &parsed, 10);
        return parsed == trim(text).size() ? value : fallback;
    } catch (...) {
        return fallback;
    }
}

std::optional<double> decimal(const std::string& value, bool allowEmpty = false)
{
    const auto text = trim(value);
    if (text.empty()) {
        return allowEmpty ? std::optional<double>(0.0) : std::nullopt;
    }
    try {
        std::size_t parsed = 0;
        const auto number = std::stod(text, &parsed);
        if (parsed != text.size() || !std::isfinite(number)) {
            return std::nullopt;
        }
        return number;
    } catch (...) {
        return std::nullopt;
    }
}

std::string fixed(double value, int decimals)
{
    if (std::fabs(value) < 0.0000001) {
        value = 0.0;
    }
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals) << value;
    return stream.str();
}

std::string quantityText(double value)
{
    auto text = fixed(value, 4);
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    return text.empty() ? "0" : text;
}

OtcMapAcMultiChannelIssue issue(std::string code, std::string field, std::string message)
{
    OtcMapAcMultiChannelIssue result;
    result.code = std::move(code);
    result.field = std::move(field);
    result.message = std::move(message);
    return result;
}

OtcMapAcMultiChannelDraftResult rejected(std::string code, std::string field, std::string message)
{
    OtcMapAcMultiChannelDraftResult result;
    result.issues.push_back(issue(std::move(code), std::move(field), std::move(message)));
    result.issueCodes.push_back(result.issues.back().code);
    return result;
}

bool containsToken(std::string text, const std::vector<std::string>& tokens)
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

std::string tokenSurface(const OtcMapAcMultiChannelDraftRequest& request)
{
    std::ostringstream stream;
    stream << request.accountId << ' ' << request.portfolioId << ' ' << request.strategyCode << ' '
           << request.idempotencyKey << ' ' << request.otcMapDigest << ' ' << request.userNote;
    for (const auto& channel : request.channels) {
        stream << ' ' << channel.instrumentCode << ' ' << channel.fundClass;
    }
    return stream.str();
}

bool validateChannelDecimals(const OtcMapAcChannel& channel, std::string& code, std::string& field)
{
    const auto nav = decimal(channel.navText);
    if (!nav.has_value() || *nav <= 0.0) {
        code = "OTCMAP_INVALID_NAV";
        field = "channels.navText";
        return false;
    }
    for (const auto& pair : {
             std::pair<const char*, std::string>{"dailyBuyLimitText", channel.dailyBuyLimitText},
             std::pair<const char*, std::string>{"dailySellLimitText", channel.dailySellLimitText},
             std::pair<const char*, std::string>{"minBuyAmountText", channel.minBuyAmountText},
             std::pair<const char*, std::string>{"minSellAmountText", channel.minSellAmountText},
             std::pair<const char*, std::string>{"holdingQuantityText", channel.holdingQuantityText},
             std::pair<const char*, std::string>{"holdingCostText", channel.holdingCostText},
         }) {
        const auto value = decimal(pair.second, true);
        if (!value.has_value() || *value < 0.0) {
            code = std::string("OTCMAP_INVALID_") + upper(pair.first);
            field = std::string("channels.") + pair.first;
            return false;
        }
    }
    return true;
}

OtcMapAcMultiChannelDraftResult validateRequest(
    const OtcMapAcMultiChannelDraftRequest& request)
{
    if (blank(request.strategyCode)) {
        return rejected("OTCMAP_MISSING_STRATEGY_CODE", "strategyCode", "strategyCode is required.");
    }
    if (request.channels.empty()) {
        return rejected("OTCMAP_NO_CHANNELS", "channels", "At least one OTCMap channel is required.");
    }
    if (containsToken(tokenSurface(request), {"password", "credential", "secret"})) {
        return rejected("OTCMAP_SENSITIVE_TOKEN", "payload", "Sensitive tokens are not accepted.");
    }
    if (containsToken(tokenSurface(request), {"broker", "order.submit", "real_order"})) {
        return rejected("OTCMAP_BROKER_ORDER_TOKEN", "payload", "Broker order tokens are not accepted.");
    }
    if (containsToken(tokenSurface(request), {"endpoint", "http://", "https://"})) {
        return rejected("OTCMAP_CREDENTIAL_ENDPOINT_TOKEN", "payload", "Endpoint tokens are not accepted.");
    }
    if (containsToken(tokenSurface(request), {"automatic_trading", "automaticTrading", "handsfree", "unsupervised"})) {
        return rejected("OTCMAP_AUTOMATIC_TRADING_TOKEN", "payload", "Automatic trading tokens are not accepted.");
    }
    const auto side = upper(trim(request.side));
    if (side != "BUY" && side != "SELL") {
        return rejected("OTCMAP_UNSUPPORTED_SIDE", "side", "side must be BUY or SELL.");
    }
    if (side == "BUY") {
        const auto target = decimal(request.targetAmountText);
        const auto cash = decimal(request.cashBalanceText);
        if (!target.has_value() || *target <= 0.0) {
            return rejected("OTCMAP_INVALID_TARGET_AMOUNT", "targetAmountText", "targetAmountText must be positive.");
        }
        if (!cash.has_value() || *cash < 0.0) {
            return rejected("OTCMAP_INVALID_CASH", "cashBalanceText", "cashBalanceText must be non-negative.");
        }
    } else {
        const auto requested = decimal(request.requestedSellQuantityText);
        const auto principalBase = decimal(request.principalBaseText);
        const auto holdingCost = decimal(request.holdingCostText, true);
        const auto baseRatio = decimal(request.basePositionRatioText, true);
        if (!requested.has_value() || *requested <= 0.0) {
            return rejected("OTCMAP_INVALID_SELL_QUANTITY", "requestedSellQuantityText", "requestedSellQuantityText must be positive.");
        }
        if (!principalBase.has_value() || *principalBase < 0.0 || !holdingCost.has_value()
            || *holdingCost < 0.0 || !baseRatio.has_value() || *baseRatio < 0.0 || *baseRatio > 1.0) {
            return rejected("OTCMAP_INVALID_BASE_PROTECTION", "basePositionRatioText", "Base protection inputs are invalid.");
        }
    }
    for (const auto& channel : request.channels) {
        if (channel.fundClass != "A" && channel.fundClass != "C") {
            return rejected("OTCMAP_UNSUPPORTED_FUND_CLASS", "channels.fundClass", "Only A and C classes are supported.");
        }
        std::string code;
        std::string field;
        if (!validateChannelDecimals(channel, code, field)) {
            return rejected(std::move(code), std::move(field), "Channel numeric fields are invalid.");
        }
    }
    OtcMapAcMultiChannelDraftResult ok;
    ok.status = "VALID";
    ok.accepted = true;
    return ok;
}

void finalizeResult(OtcMapAcMultiChannelDraftResult& result)
{
    double quantity = 0.0;
    double amount = 0.0;
    for (const auto& leg : result.legs) {
        quantity += decimal(leg.quantityText, true).value_or(0.0);
        amount += decimal(leg.amountText, true).value_or(0.0);
    }
    result.totalQuantityText = quantityText(quantity);
    result.totalAmountText = fixed(amount, 2);
    result.totalNetCashImpactText = result.side == "BUY" ? fixed(-amount, 2) : fixed(amount, 2);
    result.accepted = !result.legs.empty();
    result.eligibleForTradeDraft = result.accepted;
    result.status = result.accepted ? "DRAFT_ELIGIBLE" : "REJECTED";
}

OtcMapAcMultiChannelDraftResult buildBuy(const OtcMapAcMultiChannelDraftRequest& request)
{
    std::vector<OtcMapAcChannel> channels;
    for (const auto& channel : request.channels) {
        if (channel.enabled && channel.supportsBuy) {
            channels.push_back(channel);
        }
    }
    if (channels.empty()) {
        return rejected("OTCMAP_NO_BUY_SUPPORTED_CHANNELS", "channels", "No enabled channel supports BUY.");
    }
    std::sort(channels.begin(), channels.end(), [](const auto& left, const auto& right) {
        return left.priority < right.priority;
    });

    double remaining = std::min(
        decimal(request.targetAmountText).value_or(0.0),
        decimal(request.cashBalanceText).value_or(0.0));
    OtcMapAcMultiChannelDraftResult result;
    result.side = "BUY";
    result.cashLimitApplied =
        decimal(request.cashBalanceText).value_or(0.0) + 0.0000001 < decimal(request.targetAmountText).value_or(0.0);
    bool belowMinOnly = true;
    for (const auto& channel : channels) {
        if (remaining <= 0.000001) {
            break;
        }
        const auto nav = decimal(channel.navText).value_or(0.0);
        const auto limit = decimal(channel.dailyBuyLimitText, true).value_or(0.0);
        const auto minAmount = decimal(channel.minBuyAmountText, true).value_or(0.0);
        const double capped = limit > 0.0 ? std::min(remaining, limit) : remaining;
        const double amount = std::floor(capped * 100.0 + 0.000001) / 100.0;
        if (amount + 0.000001 < minAmount) {
            continue;
        }
        belowMinOnly = false;
        OtcMapAcMultiChannelDraftLeg leg;
        leg.instrumentId = channel.instrumentId;
        leg.instrumentCode = channel.instrumentCode;
        leg.fundClass = channel.fundClass;
        leg.side = "BUY";
        leg.quantityText = quantityText(amount / nav);
        leg.amountText = fixed(amount, 2);
        leg.navText = fixed(nav, 4);
        leg.netCashImpactText = fixed(-amount, 2);
        leg.currency = channel.currency;
        leg.priority = channel.priority;
        result.legs.push_back(std::move(leg));
        remaining -= amount;
    }
    if (result.legs.empty()) {
        return rejected(
            belowMinOnly ? "OTCMAP_BELOW_MIN_AMOUNT" : "OTCMAP_NO_BUY_ALLOCATION",
            "channels.minBuyAmountText",
            "No BUY channel can satisfy the requested allocation.");
    }
    finalizeResult(result);
    return result;
}

OtcMapAcMultiChannelDraftResult buildSell(const OtcMapAcMultiChannelDraftRequest& request)
{
    std::vector<OtcMapAcChannel> channels;
    for (const auto& channel : request.channels) {
        if (channel.enabled && channel.supportsSell
            && decimal(channel.holdingQuantityText, true).value_or(0.0) > 0.0) {
            channels.push_back(channel);
        }
    }
    if (channels.empty()) {
        return rejected("OTCMAP_NO_SELL_SUPPORTED_CHANNELS", "channels", "No enabled channel supports SELL with holdings.");
    }
    std::sort(channels.begin(), channels.end(), [](const auto& left, const auto& right) {
        if (left.fundClass != right.fundClass) {
            return left.fundClass == "C";
        }
        return left.priority < right.priority;
    });

    OtcMapAcMultiChannelDraftResult result;
    result.side = "SELL";
    double remainingQuantity = decimal(request.requestedSellQuantityText).value_or(0.0);
    double currentCost = decimal(request.holdingCostText, true).value_or(0.0);
    if (currentCost <= 0.0) {
        for (const auto& channel : channels) {
            currentCost += decimal(channel.holdingCostText, true).value_or(0.0);
        }
    }
    const double baseRequired = decimal(request.principalBaseText).value_or(0.0)
        * decimal(request.basePositionRatioText, true).value_or(0.20);
    bool baseBlocked = false;
    bool belowMinOnly = true;
    for (const auto& channel : channels) {
        if (remainingQuantity <= 0.000001) {
            break;
        }
        const double nav = decimal(channel.navText).value_or(0.0);
        const double holdingQuantity = decimal(channel.holdingQuantityText, true).value_or(0.0);
        const double limitAmount = decimal(channel.dailySellLimitText, true).value_or(0.0);
        const double minAmount = decimal(channel.minSellAmountText, true).value_or(0.0);
        const double allowedAmountByBase = std::max(0.0, currentCost - baseRequired + 0.01);
        if (allowedAmountByBase <= 0.000001) {
            baseBlocked = true;
            break;
        }
        double quantity = std::min(remainingQuantity, holdingQuantity);
        if (limitAmount > 0.0) {
            quantity = std::min(quantity, limitAmount / nav);
        }
        quantity = std::min(quantity, allowedAmountByBase / nav);
        const double amount = std::floor(quantity * nav * 100.0 + 0.000001) / 100.0;
        if (amount + 0.000001 < minAmount) {
            continue;
        }
        belowMinOnly = false;
        OtcMapAcMultiChannelDraftLeg leg;
        leg.instrumentId = channel.instrumentId;
        leg.instrumentCode = channel.instrumentCode;
        leg.fundClass = channel.fundClass;
        leg.side = "SELL";
        leg.quantityText = quantityText(amount / nav);
        leg.amountText = fixed(amount, 2);
        leg.navText = fixed(nav, 4);
        leg.netCashImpactText = fixed(amount, 2);
        leg.currency = channel.currency;
        leg.priority = channel.priority;
        result.legs.push_back(std::move(leg));
        remainingQuantity -= quantity;
        currentCost -= amount;
    }
    if (result.legs.empty()) {
        auto failed = rejected(
            baseBlocked ? "OTCMAP_BASE_PROTECTION_BLOCKED"
                        : (belowMinOnly ? "OTCMAP_BELOW_MIN_AMOUNT" : "OTCMAP_NO_SELL_ALLOCATION"),
            baseBlocked ? "basePositionRatioText" : "channels.minSellAmountText",
            "No SELL channel can satisfy the requested allocation.");
        failed.baseProtectionPassed = !baseBlocked;
        return failed;
    }
    result.baseProtectionPassed = true;
    finalizeResult(result);
    return result;
}

}  // namespace

OtcMapAcMultiChannelDraftResult OtcMapAcMultiChannelDraftEngine::buildReadOnlyDraft(
    const OtcMapAcMultiChannelDraftRequest& request) const
{
    auto validation = validateRequest(request);
    if (!validation.accepted) {
        return validation;
    }
    return upper(trim(request.side)) == "BUY" ? buildBuy(request) : buildSell(request);
}

bool decodeOtcMapAcMultiChannelDraftRequestJson(
    const std::string& payloadJson,
    OtcMapAcMultiChannelDraftRequest& request,
    std::string& errorCode)
{
    const auto parsed = JsonParser(payloadJson).parse();
    if (!parsed.has_value() || parsed->type != JsonValue::Type::Object) {
        errorCode = "OTCMAP_PAYLOAD_PARSE_ERROR";
        return false;
    }
    const JsonValue* object = objectField(*parsed, "input");
    if (object == nullptr) {
        object = &*parsed;
    }
    if (object->type != JsonValue::Type::Object) {
        errorCode = "OTCMAP_INPUT_PARSE_ERROR";
        return false;
    }

    request.accountId = jsonStringField(*object, "accountId");
    request.portfolioId = jsonStringField(*object, "portfolioId");
    request.strategyId = jsonStringField(*object, "strategyId");
    request.strategyCode = jsonStringField(*object, "strategyCode");
    request.side = upper(trim(jsonStringField(*object, "side")));
    request.targetAmountText = jsonStringField(*object, "targetAmountText");
    request.cashBalanceText = jsonStringField(*object, "cashBalanceText");
    request.requestedSellQuantityText = jsonStringField(*object, "requestedSellQuantityText");
    request.principalBaseText = jsonStringField(*object, "principalBaseText");
    request.holdingCostText = jsonStringField(*object, "holdingCostText");
    request.basePositionRatioText = jsonStringField(*object, "basePositionRatioText");
    if (blank(request.basePositionRatioText)) {
        request.basePositionRatioText = "0.20";
    }
    request.idempotencyKey = jsonStringField(*object, "idempotencyKey");
    request.otcMapDigest = jsonStringField(*object, "otcMapDigest");
    request.userNote = jsonStringField(*object, "userNote");
    request.userConfirmed = jsonBoolField(*object, "userConfirmed", false);
    request.channels.clear();

    const auto* channels = objectField(*object, "channels");
    if (channels == nullptr || channels->type != JsonValue::Type::Array) {
        errorCode = "OTCMAP_CHANNELS_PARSE_ERROR";
        return false;
    }
    for (const auto& value : channels->arrayValue) {
        if (value.type != JsonValue::Type::Object) {
            errorCode = "OTCMAP_CHANNEL_PARSE_ERROR";
            return false;
        }
        OtcMapAcChannel channel;
        channel.instrumentId = jsonInt64Field(value, "instrumentId", 0);
        channel.instrumentCode = jsonStringField(value, "instrumentCode");
        channel.fundClass = upper(trim(jsonStringField(value, "fundClass")));
        channel.enabled = jsonBoolField(value, "enabled", false);
        channel.supportsBuy = jsonBoolField(value, "supportsBuy", false);
        channel.supportsSell = jsonBoolField(value, "supportsSell", false);
        channel.priority = jsonIntField(value, "priority", 0);
        channel.dailyBuyLimitText = jsonStringField(value, "dailyBuyLimitText");
        channel.dailySellLimitText = jsonStringField(value, "dailySellLimitText");
        channel.minBuyAmountText = jsonStringField(value, "minBuyAmountText");
        channel.minSellAmountText = jsonStringField(value, "minSellAmountText");
        channel.navText = jsonStringField(value, "navText");
        channel.holdingQuantityText = jsonStringField(value, "holdingQuantityText");
        channel.holdingCostText = jsonStringField(value, "holdingCostText");
        channel.currency = jsonStringField(value, "currency");
        if (blank(channel.currency)) {
            channel.currency = "CNY";
        }
        request.channels.push_back(std::move(channel));
    }
    return true;
}

}  // namespace etfdt::strategy_engine
