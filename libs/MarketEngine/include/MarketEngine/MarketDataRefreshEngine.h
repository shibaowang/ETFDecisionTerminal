#pragma once

#include "MarketEngine/MarketDataProvider.h"

#include <string>
#include <vector>

namespace etfdt::market_engine {

struct MarketInstrumentRefreshResult final {
    std::string instrumentCode;
    std::string instrumentType;
    std::string currentPriceText;
    std::string previousCloseText;
    std::string historicalHighText;
    std::string displayedHighText;
    std::string historicalHighDate;
    std::string drawdownFromHighText;
    std::string premiumDiscountText;
    std::string dataQualityStatus = "DATA_UNAVAILABLE";
    std::string providerSource;
    bool quoteAccepted = false;
    bool historicalHighAccepted = false;
    bool stale = false;
    std::vector<std::string> issueCodes;
};

struct MarketRefreshResult final {
    std::string status = "REJECTED";
    std::string dataQualityStatus = "DATA_UNAVAILABLE";
    bool accepted = false;
    bool quoteAccepted = false;
    bool historicalHighAccepted = false;
    std::string instrumentCode;
    std::string instrumentType;
    std::string currentPriceText;
    std::string previousCloseText;
    std::string historicalHighText;
    std::string displayedHighText;
    std::string historicalHighDate;
    std::string drawdownFromHighText;
    std::string premiumDiscountText;
    bool stale = false;
    bool partial = false;
    bool providerDisabled = false;
    bool liveProviderDisabledByDefault = true;
    bool liveProviderImplemented = false;
    bool liveProviderDeferredForSafety = true;
    bool readOnlyNoWrite = true;
    bool productionDbTouched = false;
    bool tradeLogRowsWrittenByMarketData = false;
    bool cashAdjustmentRowsWrittenByMarketData = false;
    bool brokerOrderSubmitted = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool realOrderPlacement = false;
    bool automaticTrading = false;
    bool networkAccess = false;
    bool rawUrlExposed = false;
    bool rawResponseExposed = false;
    bool exactHostAllowlistEnforced = true;
    bool batchRequestsOnly = true;
    bool perHostRateLimitEnforced = true;
    bool historyHighDailyCacheEnforced = true;
    bool historyHighFailureCircuitBreakerEnforced = true;
    bool noParallelSameHostRequests = true;
    std::string providerSource;
    std::vector<std::string> issueCodes;
    std::vector<MarketInstrumentRefreshResult> instruments;
};

class MarketDataRefreshEngine final {
public:
    [[nodiscard]] MarketRefreshResult refreshReadOnly(
        const MarketRefreshInput& input,
        MarketDataProvider& provider) const;
};

}  // namespace etfdt::market_engine
