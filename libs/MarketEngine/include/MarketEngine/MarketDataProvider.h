#pragma once

#include <string>
#include <vector>

namespace etfdt::market_engine {

struct MarketInstrumentKey final {
    std::string instrumentCode;
    std::string instrumentType;
    std::string exchange;
    std::string providerSymbol;
    std::string provider;
};

struct MarketQuote final {
    MarketInstrumentKey key;
    std::string quoteTimeUtc;
    std::string priceText;
    std::string previousCloseText;
    std::string openText;
    std::string highText;
    std::string lowText;
    std::string volumeText;
    std::string currency = "CNY";
    std::string source;
    std::string dataQualityStatus = "DATA_UNAVAILABLE";
};

struct MarketHistoryBar final {
    std::string instrumentCode;
    std::string date;
    std::string highText;
    std::string closeText;
    std::string source;
};

struct MarketDataIssue final {
    std::string code;
    std::string field;
    std::string message;
    bool blocking = true;
};

struct MarketRefreshInput final {
    std::string providerMode = "fixture";
    bool liveMarketDataEnabled = false;
    int maxQuoteAgeSeconds = 900;
    std::string nowUtc = "2026-06-12T00:00:00Z";
    std::string referenceValueText;
    std::string requestDigest;
    std::string rawPayloadForSafetyScan;
    std::vector<MarketInstrumentKey> instruments;
};

struct MarketProviderSnapshot final {
    bool providerDisabled = false;
    bool liveProviderDeferredForSafety = false;
    bool networkAccess = false;
    bool rawUrlExposed = false;
    bool rawResponseExposed = false;
    bool exactHostAllowlistEnforced = true;
    bool batchRequestsOnly = true;
    bool perHostRateLimitEnforced = true;
    bool historyHighDailyCacheEnforced = true;
    bool historyFailureCircuitBreakerEnforced = true;
    bool noParallelSameHostRequests = true;
    std::string source = "fixture";
    std::vector<MarketQuote> quotes;
    std::vector<MarketHistoryBar> historyBars;
    std::vector<MarketDataIssue> issues;
};

class MarketDataProvider {
public:
    virtual ~MarketDataProvider() = default;
    [[nodiscard]] virtual MarketProviderSnapshot fetchReadOnly(
        const MarketRefreshInput& input) = 0;
};

[[nodiscard]] bool decodeMarketRefreshInputJson(
    const std::string& payloadJson,
    MarketRefreshInput& input,
    std::string& errorCode);

}  // namespace etfdt::market_engine
