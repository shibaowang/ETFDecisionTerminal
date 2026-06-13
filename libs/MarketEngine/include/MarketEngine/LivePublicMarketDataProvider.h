#pragma once

#include "MarketEngine/MarketDataProvider.h"
#include "MarketEngine/MarketDataSafetyPolicy.h"

#include <filesystem>
#include <map>
#include <string>

namespace etfdt::market_engine {

class LivePublicMarketDataProvider final : public MarketDataProvider {
public:
    explicit LivePublicMarketDataProvider(
        bool liveMarketDataEnabled = true,
        std::filesystem::path cachePath = ".local/daily_use/cache/market_cache.json");

    [[nodiscard]] MarketProviderSnapshot fetchReadOnly(
        const MarketRefreshInput& input) override;

private:
    [[nodiscard]] std::string fetchUrl(
        const std::string& url,
        int timeoutMs,
        MarketProviderSnapshot& snapshot) const;
    [[nodiscard]] bool authorizeUrl(
        const std::string& url,
        MarketProviderSnapshot& snapshot) const;
    [[nodiscard]] std::string secidFor(const MarketInstrumentKey& key) const;
    [[nodiscard]] std::string providerSymbolFor(const MarketInstrumentKey& key) const;
    void fetchQuotes(const MarketRefreshInput& input, MarketProviderSnapshot& snapshot);
    void fetchHistory(const MarketRefreshInput& input, MarketProviderSnapshot& snapshot);
    void loadCache(MarketProviderSnapshot& snapshot) const;
    void saveCache(const MarketProviderSnapshot& snapshot) const;

    bool liveMarketDataEnabled_ = true;
    std::filesystem::path cachePath_;
    MarketDataSafetyPolicy safetyPolicy_;
};

}  // namespace etfdt::market_engine
