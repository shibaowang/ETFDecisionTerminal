#include "MarketEngine/LivePublicMarketDataProvider.h"

#include <QByteArray>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <QUrlQuery>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <mutex>
#include <sstream>
#include <utility>

namespace etfdt::market_engine {
namespace {

constexpr const char* kDailyUseMarketCacheFile = ".local/daily_use/cache/market_cache.json";

std::mutex g_marketNetworkMutex;
MarketDataRateLimiter g_rateLimiter;
MarketDataCircuitBreaker g_historyCircuitBreaker;
MarketDataInMemoryCache g_historyDailyCache;

long long nowSeconds()
{
    return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

std::string todayFromInput(const MarketRefreshInput& input)
{
    return input.nowUtc.size() >= 10U ? input.nowUtc.substr(0, 10) : "unknown-day";
}

std::string lower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

bool startsWith(const std::string& text, const std::string& prefix)
{
    return text.rfind(prefix, 0) == 0;
}

std::string marketPrefixForCode(const MarketInstrumentKey& key)
{
    const auto exchange = lower(key.exchange);
    if (exchange == "sh" || exchange == "sse") {
        return "1";
    }
    if (exchange == "sz" || exchange == "szse") {
        return "0";
    }
    if (startsWith(key.providerSymbol, "sh")) {
        return "1";
    }
    if (startsWith(key.providerSymbol, "sz")) {
        return "0";
    }
    if (!key.instrumentCode.empty()
        && (key.instrumentCode.front() == '5' || key.instrumentCode.front() == '6')) {
        return "1";
    }
    return "0";
}

std::string stringField(const QJsonObject& object, const char* key, std::string fallback = {})
{
    const auto value = object.value(QString::fromUtf8(key));
    if (value.isString()) {
        return value.toString().toStdString();
    }
    if (value.isDouble()) {
        std::ostringstream stream;
        stream << value.toDouble();
        return stream.str();
    }
    return fallback;
}

std::string formatNumber(double value, int precision)
{
    if (std::fabs(value) < 0.0000005) {
        value = 0.0;
    }
    std::ostringstream stream;
    stream.setf(std::ios::fixed);
    stream.precision(precision);
    stream << value;
    return stream.str();
}

double numericField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    if (value.isDouble()) {
        return value.toDouble();
    }
    if (value.isString()) {
        bool ok = false;
        const auto parsed = value.toString().toDouble(&ok);
        return ok ? parsed : 0.0;
    }
    return 0.0;
}

void addIssue(
    MarketProviderSnapshot& snapshot,
    std::string code,
    std::string field,
    std::string message,
    bool blocking = true)
{
    snapshot.issues.push_back({
        std::move(code),
        std::move(field),
        std::move(message),
        blocking,
    });
}

std::vector<std::string> split(const std::string& text, char delimiter)
{
    std::vector<std::string> parts;
    std::string current;
    for (const auto ch : text) {
        if (ch == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(ch);
        }
    }
    parts.push_back(current);
    return parts;
}

}  // namespace

LivePublicMarketDataProvider::LivePublicMarketDataProvider(
    bool liveMarketDataEnabled,
    std::filesystem::path cachePath)
    : liveMarketDataEnabled_(liveMarketDataEnabled)
    , cachePath_(cachePath.empty() ? std::filesystem::path(kDailyUseMarketCacheFile)
                                   : std::move(cachePath))
{
}

MarketProviderSnapshot LivePublicMarketDataProvider::fetchReadOnly(
    const MarketRefreshInput& input)
{
    MarketProviderSnapshot snapshot;
    snapshot.source = "live-public-market";
    snapshot.liveProviderImplemented = true;
    snapshot.liveProviderDeferredForSafety = false;
    snapshot.providerDisabled = !liveMarketDataEnabled_;
    snapshot.networkAccess = false;
    snapshot.rawUrlExposed = false;
    snapshot.rawResponseExposed = false;
    snapshot.exactHostAllowlistEnforced = true;
    snapshot.batchRequestsOnly = true;
    snapshot.perHostRateLimitEnforced = true;
    snapshot.historyHighDailyCacheEnforced = true;
    snapshot.historyFailureCircuitBreakerEnforced = true;
    snapshot.noParallelSameHostRequests = true;

    loadCache(snapshot);

    if (!liveMarketDataEnabled_) {
        addIssue(
            snapshot,
            "MARKET_DATA_LIVE_PROVIDER_DISABLED_BY_DEFAULT",
            "provider",
            "Live public market data provider is disabled by explicit request.");
        return snapshot;
    }

    if (input.instruments.empty()) {
        addIssue(
            snapshot,
            "MARKET_DATA_NO_REAL_HOLDING_INSTRUMENTS",
            "instruments",
            "No real daily-use holding instruments were available for live market refresh.");
        return snapshot;
    }

    const auto seconds = nowSeconds();
    const auto batchKey = input.requestDigest.empty() ? "daily-use-startup" : input.requestDigest;
    if (!g_rateLimiter.allowBatch("live-public-market", batchKey, seconds)) {
        addIssue(
            snapshot,
            "MARKET_DATA_RATE_LIMITED_30_SECONDS",
            "rateLimit",
            "Live public market data refresh is rate limited to at least 30 seconds per batch.",
            false);
        snapshot.stale = !snapshot.quotes.empty() || !snapshot.historyBars.empty();
        return snapshot;
    }

    fetchQuotes(input, snapshot);
    fetchHistory(input, snapshot);
    saveCache(snapshot);
    return snapshot;
}

bool LivePublicMarketDataProvider::authorizeUrl(
    const std::string& url,
    MarketProviderSnapshot& snapshot) const
{
    const QUrl qurl(QString::fromStdString(url));
    const auto decision = safetyPolicy_.authorizeUrl(
        qurl.scheme().toStdString(),
        qurl.host().toStdString(),
        qurl.path().toStdString());
    if (!decision.allowed) {
        addIssue(
            snapshot,
            decision.issueCode,
            "marketUrl",
            "Public market data request was rejected by exact host/path allowlist.");
        return false;
    }
    return true;
}

std::string LivePublicMarketDataProvider::fetchUrl(
    const std::string& url,
    int timeoutMs,
    MarketProviderSnapshot& snapshot) const
{
    if (!authorizeUrl(url, snapshot)) {
        return {};
    }

    std::lock_guard<std::mutex> sameHostGuard(g_marketNetworkMutex);
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl(QString::fromStdString(url)));
    request.setTransferTimeout(timeoutMs);
    request.setHeader(QNetworkRequest::UserAgentHeader, QStringLiteral("ETFDecisionTerminal/market-readonly"));

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    auto* reply = manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    if (timer.isActive()) {
        timer.stop();
    } else if (reply->isRunning()) {
        reply->abort();
    }

    if (reply->error() != QNetworkReply::NoError) {
        addIssue(
            snapshot,
            "MARKET_DATA_HTTP_REQUEST_FAILED",
            "publicMarketData",
            "Public market data request failed or timed out.");
        reply->deleteLater();
        return {};
    }

    snapshot.networkAccess = true;
    const auto bytes = reply->readAll();
    reply->deleteLater();
    return bytes.toStdString();
}

std::string LivePublicMarketDataProvider::providerSymbolFor(
    const MarketInstrumentKey& key) const
{
    if (!key.providerSymbol.empty()) {
        return key.providerSymbol;
    }
    if (marketPrefixForCode(key) == "1") {
        return "sh" + key.instrumentCode;
    }
    return "sz" + key.instrumentCode;
}

std::string LivePublicMarketDataProvider::secidFor(const MarketInstrumentKey& key) const
{
    return marketPrefixForCode(key) + "." + key.instrumentCode;
}

void LivePublicMarketDataProvider::fetchQuotes(
    const MarketRefreshInput& input,
    MarketProviderSnapshot& snapshot)
{
    std::string secids;
    for (const auto& key : input.instruments) {
        if (!secids.empty()) {
            secids += ",";
        }
        secids += secidFor(key);
    }

    QUrl eastmoney(QStringLiteral("https://push2.eastmoney.com/api/qt/ulist.np/get"));
    QUrlQuery query;
    query.addQueryItem(QStringLiteral("fltt"), QStringLiteral("2"));
    query.addQueryItem(QStringLiteral("fields"), QStringLiteral("f12,f14,f2,f3,f4,f43,f44,f45,f46,f47,f170"));
    query.addQueryItem(QStringLiteral("secids"), QString::fromStdString(secids));
    eastmoney.setQuery(query);
    const auto payload = fetchUrl(eastmoney.toString().toStdString(), 2000, snapshot);
    if (payload.empty()) {
        return;
    }

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payload), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        addIssue(
            snapshot,
            "MARKET_DATA_QUOTE_JSON_INVALID",
            "quote",
            "Public quote response could not be parsed.");
        return;
    }

    const auto data = document.object().value(QStringLiteral("data")).toObject();
    const auto diff = data.value(QStringLiteral("diff")).toArray();
    for (const auto& value : diff) {
        if (!value.isObject()) {
            continue;
        }
        const auto object = value.toObject();
        const auto code = stringField(object, "f12");
        auto found = std::find_if(input.instruments.begin(), input.instruments.end(), [&](const auto& key) {
            return key.instrumentCode == code;
        });
        if (found == input.instruments.end()) {
            continue;
        }

        MarketQuote quote;
        quote.key = *found;
        quote.quoteTimeUtc = input.nowUtc;
        const auto price = numericField(object, "f2");
        const auto previousClose = numericField(object, "f2") - numericField(object, "f4");
        quote.priceText = price > 0.0 ? formatNumber(price, 3) : stringField(object, "f43");
        quote.previousCloseText = previousClose > 0.0 ? formatNumber(previousClose, 3) : std::string {};
        quote.highText = stringField(object, "f44");
        quote.lowText = stringField(object, "f45");
        quote.openText = stringField(object, "f46");
        quote.volumeText = stringField(object, "f47");
        quote.source = "push2.eastmoney.com/api/qt/ulist.np/get";
        quote.dataQualityStatus = price > 0.0 ? "OK" : "DATA_UNAVAILABLE";
        snapshot.quotes.push_back(std::move(quote));
    }

    if (snapshot.quotes.empty()) {
        addIssue(
            snapshot,
            "MARKET_DATA_QUOTE_EMPTY",
            "quote",
            "Public quote response did not include requested instruments.");
    }

    // Supported fallback endpoints are intentionally kept behind the same safety policy.
    // Tencent: http://qt.gtimg.cn/q=sh510300
    // Sina NAV: http://hq.sinajs.cn/list=f_510300
}

void LivePublicMarketDataProvider::fetchHistory(
    const MarketRefreshInput& input,
    MarketProviderSnapshot& snapshot)
{
    const auto today = todayFromInput(input);
    const auto seconds = nowSeconds();
    for (const auto& key : input.instruments) {
        const auto secid = secidFor(key);
        if (g_historyDailyCache.hasHistorySuccessForToday(secid, today)) {
            addIssue(
                snapshot,
                "MARKET_DATA_HISTORY_DAILY_CACHE_HIT",
                "history",
                "Historical high used the daily cache for this secid.",
                false);
            continue;
        }
        if (!g_historyCircuitBreaker.allowHistoryRequest(secid, seconds)) {
            addIssue(
                snapshot,
                "MARKET_DATA_HISTORY_CIRCUIT_BREAKER_OPEN",
                "history",
                "Historical high request is circuit-broken for 10 minutes after failure.",
                false);
            continue;
        }

        QUrl eastmoney(QStringLiteral("https://push2his.eastmoney.com/api/qt/stock/kline/get"));
        QUrlQuery query;
        query.addQueryItem(QStringLiteral("secid"), QString::fromStdString(secid));
        query.addQueryItem(QStringLiteral("klt"), QStringLiteral("103"));
        query.addQueryItem(QStringLiteral("fqt"), key.instrumentType == "INDEX" ? QStringLiteral("0") : QStringLiteral("1"));
        query.addQueryItem(QStringLiteral("beg"), QStringLiteral("19900101"));
        query.addQueryItem(QStringLiteral("end"), QStringLiteral("20500101"));
        query.addQueryItem(QStringLiteral("fields1"), QStringLiteral("f1,f2,f3,f4,f5,f6"));
        query.addQueryItem(QStringLiteral("fields2"), QStringLiteral("f51,f52,f53,f54,f55,f56,f57,f58,f59,f60,f61"));
        eastmoney.setQuery(query);

        const auto payload = fetchUrl(eastmoney.toString().toStdString(), 3000, snapshot);
        if (payload.empty()) {
            g_historyCircuitBreaker.recordHistoryFailure(secid, seconds);
            continue;
        }

        QJsonParseError parseError {};
        const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payload), &parseError);
        const auto klines = document.object()
                               .value(QStringLiteral("data"))
                               .toObject()
                               .value(QStringLiteral("klines"))
                               .toArray();
        if (parseError.error != QJsonParseError::NoError || klines.isEmpty()) {
            g_historyCircuitBreaker.recordHistoryFailure(secid, seconds);
            addIssue(
                snapshot,
                "MARKET_DATA_HISTORY_JSON_INVALID",
                "history",
                "Public historical K-line response could not be parsed.");
            continue;
        }

        for (const auto& item : klines) {
            const auto parts = split(item.toString().toStdString(), ',');
            if (parts.size() < 4U) {
                continue;
            }
            MarketHistoryBar bar;
            bar.instrumentCode = key.instrumentCode;
            bar.date = parts[0];
            bar.closeText = parts[2];
            bar.highText = parts[3];
            bar.source = "push2his.eastmoney.com/api/qt/stock/kline/get;klt=103;fqt="
                + std::string(key.instrumentType == "INDEX" ? "0" : "1");
            snapshot.historyBars.push_back(std::move(bar));
        }
        g_historyDailyCache.recordHistorySuccessForToday(secid, today);
    }
}

void LivePublicMarketDataProvider::loadCache(MarketProviderSnapshot& snapshot) const
{
    QFile file(QString::fromStdString(cachePath_.generic_string()));
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return;
    }
    const auto root = document.object();
    const auto quoteArray = root.value(QStringLiteral("quotes")).toArray();
    for (const auto& value : quoteArray) {
        if (!value.isObject()) {
            continue;
        }
        const auto object = value.toObject();
        MarketQuote quote;
        quote.key.instrumentCode = stringField(object, "instrumentCode");
        quote.key.instrumentType = stringField(object, "instrumentType");
        quote.key.exchange = stringField(object, "exchange");
        quote.key.providerSymbol = stringField(object, "providerSymbol");
        quote.quoteTimeUtc = stringField(object, "quoteTimeUtc");
        quote.priceText = stringField(object, "priceText");
        quote.previousCloseText = stringField(object, "previousCloseText");
        quote.source = "daily-use-cache";
        quote.dataQualityStatus = "STALE";
        snapshot.quotes.push_back(std::move(quote));
    }
    const auto historyArray = root.value(QStringLiteral("history")).toArray();
    for (const auto& value : historyArray) {
        if (!value.isObject()) {
            continue;
        }
        const auto object = value.toObject();
        MarketHistoryBar bar;
        bar.instrumentCode = stringField(object, "instrumentCode");
        bar.date = stringField(object, "date");
        bar.highText = stringField(object, "highText");
        bar.closeText = stringField(object, "closeText");
        bar.source = "daily-use-cache";
        snapshot.historyBars.push_back(std::move(bar));
    }
    if (!snapshot.quotes.empty() || !snapshot.historyBars.empty()) {
        snapshot.stale = true;
    }
}

void LivePublicMarketDataProvider::saveCache(const MarketProviderSnapshot& snapshot) const
{
    if (snapshot.quotes.empty() && snapshot.historyBars.empty()) {
        return;
    }
    QJsonArray quotes;
    for (const auto& quote : snapshot.quotes) {
        QJsonObject object;
        object.insert(QStringLiteral("instrumentCode"), QString::fromStdString(quote.key.instrumentCode));
        object.insert(QStringLiteral("instrumentType"), QString::fromStdString(quote.key.instrumentType));
        object.insert(QStringLiteral("exchange"), QString::fromStdString(quote.key.exchange));
        object.insert(QStringLiteral("providerSymbol"), QString::fromStdString(quote.key.providerSymbol));
        object.insert(QStringLiteral("quoteTimeUtc"), QString::fromStdString(quote.quoteTimeUtc));
        object.insert(QStringLiteral("priceText"), QString::fromStdString(quote.priceText));
        object.insert(QStringLiteral("previousCloseText"), QString::fromStdString(quote.previousCloseText));
        quotes.push_back(object);
    }
    QJsonArray history;
    for (const auto& bar : snapshot.historyBars) {
        QJsonObject object;
        object.insert(QStringLiteral("instrumentCode"), QString::fromStdString(bar.instrumentCode));
        object.insert(QStringLiteral("date"), QString::fromStdString(bar.date));
        object.insert(QStringLiteral("highText"), QString::fromStdString(bar.highText));
        object.insert(QStringLiteral("closeText"), QString::fromStdString(bar.closeText));
        history.push_back(object);
    }

    QJsonObject root;
    root.insert(QStringLiteral("cacheSchema"), QStringLiteral("EPIC-289/daily-use-market-cache/v1"));
    root.insert(QStringLiteral("quotes"), quotes);
    root.insert(QStringLiteral("history"), history);

    const auto parent = cachePath_.parent_path();
    if (!parent.empty()) {
        QDir().mkpath(QString::fromStdString(parent.generic_string()));
    }
    QFile file(QString::fromStdString(cachePath_.generic_string()));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }
    file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
}

}  // namespace etfdt::market_engine
