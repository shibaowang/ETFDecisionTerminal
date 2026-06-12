#pragma once

#include "MarketEngine/MarketDataProvider.h"

#include <map>
#include <set>
#include <string>

namespace etfdt::market_engine {

struct MarketDataSafetyDecision final {
    bool allowed = false;
    std::string issueCode;
};

class MarketDataSafetyPolicy final {
public:
    [[nodiscard]] bool isAllowedHost(const std::string& host) const;
    [[nodiscard]] bool isAllowedPath(const std::string& host, const std::string& path) const;
    [[nodiscard]] bool containsForbiddenToken(const std::string& text) const;
    [[nodiscard]] MarketDataSafetyDecision authorizeUrl(
        const std::string& scheme,
        const std::string& host,
        const std::string& path) const;
};

class MarketDataRateLimiter final {
public:
    [[nodiscard]] bool allowBatch(
        const std::string& provider,
        const std::string& batchKey,
        long long nowSeconds);

private:
    std::map<std::string, long long> lastBatchAt_;
};

class MarketDataCircuitBreaker final {
public:
    void recordHistoryFailure(const std::string& secid, long long nowSeconds);
    [[nodiscard]] bool allowHistoryRequest(const std::string& secid, long long nowSeconds) const;

private:
    std::map<std::string, long long> blockedUntil_;
};

class MarketDataInMemoryCache final {
public:
    [[nodiscard]] bool hasHistorySuccessForToday(const std::string& secid, const std::string& date) const;
    void recordHistorySuccessForToday(const std::string& secid, const std::string& date);

private:
    std::set<std::string> historySuccessKeys_;
};

class LivePublicMarketDataProviderBoundary final : public MarketDataProvider {
public:
    explicit LivePublicMarketDataProviderBoundary(bool liveMarketDataEnabled = false);
    [[nodiscard]] MarketProviderSnapshot fetchReadOnly(
        const MarketRefreshInput& input) override;

private:
    bool liveMarketDataEnabled_ = false;
};

}  // namespace etfdt::market_engine
