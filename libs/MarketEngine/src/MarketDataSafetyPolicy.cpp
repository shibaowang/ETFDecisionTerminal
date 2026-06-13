#include "MarketEngine/MarketDataSafetyPolicy.h"

#include "MarketEngine/LivePublicMarketDataProvider.h"

#include <algorithm>
#include <cctype>
#include <utility>

namespace etfdt::market_engine {
namespace {

std::string lower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

}  // namespace

bool MarketDataSafetyPolicy::isAllowedHost(const std::string& host) const
{
    const auto normalized = lower(host);
    return normalized == "qt.gtimg.cn"
        || normalized == "push2.eastmoney.com"
        || normalized == "hq.sinajs.cn"
        || normalized == "push2his.eastmoney.com";
}

bool MarketDataSafetyPolicy::isAllowedPath(
    const std::string& host,
    const std::string& path) const
{
    const auto normalizedHost = lower(host);
    if (normalizedHost == "qt.gtimg.cn") {
        return path == "/q=" || path.rfind("/q=", 0) == 0;
    }
    if (normalizedHost == "push2.eastmoney.com") {
        return path == "/api/qt/ulist.np/get";
    }
    if (normalizedHost == "hq.sinajs.cn") {
        return path == "/list=" || path.rfind("/list=", 0) == 0;
    }
    if (normalizedHost == "push2his.eastmoney.com") {
        return path == "/api/qt/stock/kline/get";
    }
    return false;
}

bool MarketDataSafetyPolicy::containsForbiddenToken(const std::string& text) const
{
    const auto normalized = lower(text);
    for (const auto* token : {
             "broker_order",
             "brokerorder",
             "broker endpoint",
             "credential",
             "cookie",
             "session",
             "token=",
             "access_token",
             "password",
             "private endpoint",
             "pushplus",
             "automatic_trading",
             "auto trading",
             "real_order",
         }) {
        if (normalized.find(token) != std::string::npos) {
            return true;
        }
    }
    return false;
}

MarketDataSafetyDecision MarketDataSafetyPolicy::authorizeUrl(
    const std::string& scheme,
    const std::string& host,
    const std::string& path) const
{
    const auto normalizedScheme = lower(scheme);
    if (normalizedScheme != "http" && normalizedScheme != "https") {
        return {false, "MARKET_DATA_SCHEME_NOT_ALLOWLISTED"};
    }
    if (!isAllowedHost(host)) {
        return {false, "MARKET_DATA_HOST_NOT_ALLOWLISTED"};
    }
    if (!isAllowedPath(host, path)) {
        return {false, "MARKET_DATA_PATH_NOT_ALLOWLISTED"};
    }
    return {true, {}};
}

bool MarketDataRateLimiter::allowBatch(
    const std::string& provider,
    const std::string& batchKey,
    long long nowSeconds)
{
    const auto key = provider + "|" + batchKey;
    const auto found = lastBatchAt_.find(key);
    if (found != lastBatchAt_.end() && nowSeconds - found->second < 30) {
        return false;
    }
    lastBatchAt_[key] = nowSeconds;
    return true;
}

void MarketDataCircuitBreaker::recordHistoryFailure(
    const std::string& secid,
    long long nowSeconds)
{
    blockedUntil_[secid] = nowSeconds + 600;
}

bool MarketDataCircuitBreaker::allowHistoryRequest(
    const std::string& secid,
    long long nowSeconds) const
{
    const auto found = blockedUntil_.find(secid);
    return found == blockedUntil_.end() || nowSeconds >= found->second;
}

bool MarketDataInMemoryCache::hasHistorySuccessForToday(
    const std::string& secid,
    const std::string& date) const
{
    return historySuccessKeys_.find(secid + "|" + date) != historySuccessKeys_.end();
}

void MarketDataInMemoryCache::recordHistorySuccessForToday(
    const std::string& secid,
    const std::string& date)
{
    historySuccessKeys_.insert(secid + "|" + date);
}

LivePublicMarketDataProviderBoundary::LivePublicMarketDataProviderBoundary(
    bool liveMarketDataEnabled)
    : liveMarketDataEnabled_(liveMarketDataEnabled)
{
}

MarketProviderSnapshot LivePublicMarketDataProviderBoundary::fetchReadOnly(
    const MarketRefreshInput& input)
{
    LivePublicMarketDataProvider provider(liveMarketDataEnabled_);
    return provider.fetchReadOnly(input);
}

}  // namespace etfdt::market_engine
