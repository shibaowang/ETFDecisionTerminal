#include "MarketEngine/MarketDataRefreshEngine.h"

#include "MarketEngine/MarketDataSafetyPolicy.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <utility>

namespace etfdt::market_engine {
namespace {

std::string stringField(const QJsonObject& object, const char* key, std::string fallback = {})
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::move(fallback);
}

bool boolField(const QJsonObject& object, const char* key, bool fallback = false)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isBool() ? value.toBool() : fallback;
}

int intField(const QJsonObject& object, const char* key, int fallback = 0)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isDouble() ? value.toInt() : fallback;
}

bool parseNumber(const std::string& text, double& value)
{
    try {
        std::size_t parsed = 0;
        value = std::stod(text, &parsed);
        return parsed == text.size() && std::isfinite(value);
    } catch (...) {
        return false;
    }
}

std::string formatDecimal(double value, int precision)
{
    if (std::fabs(value) < 0.0000005) {
        value = 0.0;
    }
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(precision) << value;
    return stream.str();
}

void addIssue(MarketRefreshResult& result, std::string code)
{
    if (std::find(result.issueCodes.begin(), result.issueCodes.end(), code) == result.issueCodes.end()) {
        result.issueCodes.push_back(std::move(code));
    }
}

void addIssue(MarketInstrumentRefreshResult& result, std::string code)
{
    if (std::find(result.issueCodes.begin(), result.issueCodes.end(), code) == result.issueCodes.end()) {
        result.issueCodes.push_back(std::move(code));
    }
}

const MarketQuote* findQuote(
    const std::vector<MarketQuote>& quotes,
    const std::string& instrumentCode)
{
    for (const auto& quote : quotes) {
        if (quote.key.instrumentCode == instrumentCode) {
            return &quote;
        }
    }
    return nullptr;
}

std::vector<MarketHistoryBar> historyFor(
    const std::vector<MarketHistoryBar>& bars,
    const std::string& instrumentCode)
{
    std::vector<MarketHistoryBar> result;
    for (const auto& bar : bars) {
        if (bar.instrumentCode == instrumentCode) {
            result.push_back(bar);
        }
    }
    return result;
}

MarketInstrumentRefreshResult evaluateInstrument(
    const MarketInstrumentKey& key,
    const MarketQuote* quote,
    const std::vector<MarketHistoryBar>& history,
    const MarketRefreshInput& input,
    const std::string& providerSource)
{
    MarketInstrumentRefreshResult result;
    result.instrumentCode = key.instrumentCode;
    result.instrumentType = key.instrumentType;
    result.providerSource = providerSource;

    const auto instrumentType = key.instrumentType;
    if (instrumentType != "ETF" && instrumentType != "INDEX" && instrumentType != "OTC_FUND") {
        result.dataQualityStatus = "UNSUPPORTED_INSTRUMENT";
        addIssue(result, "MARKET_DATA_UNSUPPORTED_INSTRUMENT");
        return result;
    }

    if (quote == nullptr) {
        result.dataQualityStatus = "DATA_UNAVAILABLE";
        addIssue(result, "MARKET_DATA_QUOTE_UNAVAILABLE");
        return result;
    }

    result.currentPriceText = quote->priceText;
    result.previousCloseText = quote->previousCloseText;
    result.providerSource = quote->source.empty() ? providerSource : quote->source;

    double currentPrice = 0.0;
    if (!parseNumber(quote->priceText, currentPrice) || currentPrice <= 0.0) {
        result.dataQualityStatus = "DATA_UNAVAILABLE";
        addIssue(result, quote->priceText.empty() ? "MARKET_DATA_PRICE_MISSING"
                                                  : "MARKET_DATA_PRICE_INVALID");
        return result;
    }

    result.quoteAccepted = true;
    result.dataQualityStatus = quote->dataQualityStatus.empty() ? "OK" : quote->dataQualityStatus;
    if (result.dataQualityStatus == "STALE") {
        result.stale = true;
        addIssue(result, "MARKET_DATA_STALE_QUOTE");
    }

    double historicalHigh = 0.0;
    std::string highDate;
    for (const auto& bar : history) {
        double candidate = 0.0;
        if (!parseNumber(bar.highText, candidate) || candidate <= 0.0) {
            addIssue(result, "MARKET_DATA_HISTORY_HIGH_INVALID");
            continue;
        }
        if (candidate > historicalHigh) {
            historicalHigh = candidate;
            highDate = bar.date;
        }
    }

    if (historicalHigh > 0.0) {
        result.historicalHighAccepted = true;
        result.historicalHighText = formatDecimal(historicalHigh, 3);
        result.displayedHighText = formatDecimal(std::max(historicalHigh, currentPrice), 3);
        result.historicalHighDate =
            currentPrice > historicalHigh && !quote->quoteTimeUtc.empty()
                ? quote->quoteTimeUtc.substr(0, 10)
                : highDate;
        result.drawdownFromHighText =
            formatDecimal((currentPrice - historicalHigh) / historicalHigh, 4);
    } else {
        result.dataQualityStatus = result.quoteAccepted ? "PARTIAL" : "DATA_UNAVAILABLE";
        addIssue(result, history.empty() ? "MARKET_DATA_HISTORY_UNAVAILABLE"
                                         : "MARKET_DATA_HISTORY_HIGH_INVALID");
    }

    double reference = 0.0;
    if (!input.referenceValueText.empty() && parseNumber(input.referenceValueText, reference)
        && reference > 0.0) {
        result.premiumDiscountText = formatDecimal((currentPrice - reference) / reference, 4);
    } else {
        result.premiumDiscountText.clear();
    }

    if (result.issueCodes.empty() && result.dataQualityStatus != "STALE") {
        result.dataQualityStatus = "OK";
    }
    return result;
}

}  // namespace

bool decodeMarketRefreshInputJson(
    const std::string& payloadJson,
    MarketRefreshInput& input,
    std::string& errorCode)
{
    input = MarketRefreshInput {};
    input.rawPayloadForSafetyScan = payloadJson;

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payloadJson), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        errorCode = "MARKET_DATA_INVALID_JSON";
        return false;
    }

    const auto root = document.object();
    const auto object = root.value(QStringLiteral("input")).isObject()
        ? root.value(QStringLiteral("input")).toObject()
        : root;

    input.providerMode = stringField(object, "providerMode", "fixture");
    input.liveMarketDataEnabled = boolField(object, "liveMarketDataEnabled", false);
    input.maxQuoteAgeSeconds = intField(object, "maxQuoteAgeSeconds", 900);
    input.nowUtc = stringField(object, "nowUtc", input.nowUtc);
    input.referenceValueText = stringField(object, "referenceValueText");
    input.requestDigest = stringField(object, "requestDigest");

    const auto instruments = object.value(QStringLiteral("instruments")).toArray();
    for (const auto& value : instruments) {
        if (!value.isObject()) {
            continue;
        }
        const auto item = value.toObject();
        MarketInstrumentKey key;
        key.instrumentCode = stringField(item, "instrumentCode");
        key.instrumentType = stringField(item, "instrumentType");
        key.exchange = stringField(item, "exchange");
        key.providerSymbol = stringField(item, "providerSymbol");
        key.provider = stringField(item, "provider", input.providerMode);
        if (!key.instrumentCode.empty()) {
            input.instruments.push_back(std::move(key));
        }
    }

    if (input.instruments.empty()) {
        errorCode = "MARKET_DATA_NO_INSTRUMENTS";
        return false;
    }
    return true;
}

MarketRefreshResult MarketDataRefreshEngine::refreshReadOnly(
    const MarketRefreshInput& input,
    MarketDataProvider& provider) const
{
    MarketRefreshResult result;
    result.providerSource = input.providerMode;

    if (MarketDataSafetyPolicy{}.containsForbiddenToken(input.rawPayloadForSafetyScan)) {
        result.status = "REJECTED";
        result.dataQualityStatus = "DATA_UNAVAILABLE";
        addIssue(result, "MARKET_DATA_FORBIDDEN_TOKEN");
        return result;
    }

    auto snapshot = provider.fetchReadOnly(input);
    result.providerDisabled = snapshot.providerDisabled;
    result.liveProviderImplemented = snapshot.liveProviderImplemented;
    result.liveProviderDisabledByDefault = snapshot.providerDisabled;
    result.liveProviderDeferredForSafety = snapshot.liveProviderDeferredForSafety;
    result.networkAccess = snapshot.networkAccess;
    result.rawUrlExposed = snapshot.rawUrlExposed;
    result.rawResponseExposed = snapshot.rawResponseExposed;
    result.exactHostAllowlistEnforced = snapshot.exactHostAllowlistEnforced;
    result.batchRequestsOnly = snapshot.batchRequestsOnly;
    result.perHostRateLimitEnforced = snapshot.perHostRateLimitEnforced;
    result.historyHighDailyCacheEnforced = snapshot.historyHighDailyCacheEnforced;
    result.historyHighFailureCircuitBreakerEnforced =
        snapshot.historyFailureCircuitBreakerEnforced;
    result.noParallelSameHostRequests = snapshot.noParallelSameHostRequests;
    result.providerSource = snapshot.source;
    result.stale = snapshot.stale;

    for (const auto& issue : snapshot.issues) {
        addIssue(result, issue.code);
    }

    for (const auto& key : input.instruments) {
        auto instrument = evaluateInstrument(
            key,
            findQuote(snapshot.quotes, key.instrumentCode),
            historyFor(snapshot.historyBars, key.instrumentCode),
            input,
            snapshot.source);
        result.instruments.push_back(instrument);
    }

    if (!result.instruments.empty()) {
        const auto& first = result.instruments.front();
        result.instrumentCode = first.instrumentCode;
        result.instrumentType = first.instrumentType;
        result.currentPriceText = first.currentPriceText;
        result.previousCloseText = first.previousCloseText;
        result.historicalHighText = first.historicalHighText;
        result.displayedHighText = first.displayedHighText;
        result.historicalHighDate = first.historicalHighDate;
        result.drawdownFromHighText = first.drawdownFromHighText;
        result.premiumDiscountText = first.premiumDiscountText;
        result.providerSource = first.providerSource;
        result.quoteAccepted = first.quoteAccepted;
        result.historicalHighAccepted = first.historicalHighAccepted;
        result.stale = first.stale;
        for (const auto& code : first.issueCodes) {
            addIssue(result, code);
        }
    }

    bool anyQuote = false;
    bool anyHistory = false;
    bool anyStale = false;
    for (const auto& instrument : result.instruments) {
        anyQuote = anyQuote || instrument.quoteAccepted;
        anyHistory = anyHistory || instrument.historicalHighAccepted;
        anyStale = anyStale || instrument.stale;
    }

    if (result.providerDisabled) {
        result.status = "REJECTED";
        result.dataQualityStatus = "PROVIDER_DISABLED";
        addIssue(result, "MARKET_DATA_PROVIDER_DISABLED");
        return result;
    }

    if (anyQuote && anyHistory && !anyStale) {
        result.status = "OK";
        result.dataQualityStatus = "OK";
        result.accepted = true;
        return result;
    }
    if (anyQuote && anyStale) {
        result.status = "STALE";
        result.dataQualityStatus = "STALE";
        result.accepted = true;
        result.stale = true;
        addIssue(result, "MARKET_DATA_STALE_QUOTE");
        return result;
    }
    if (anyQuote) {
        result.status = "PARTIAL";
        result.dataQualityStatus = "PARTIAL";
        result.accepted = true;
        result.partial = true;
        addIssue(result, "MARKET_DATA_PARTIAL_HISTORY");
        return result;
    }

    result.status = "REJECTED";
    result.dataQualityStatus = result.issueCodes.empty() ? "DATA_UNAVAILABLE" : "DATA_UNAVAILABLE";
    if (result.issueCodes.empty()) {
        addIssue(result, "MARKET_DATA_UNAVAILABLE");
    }
    return result;
}

}  // namespace etfdt::market_engine
