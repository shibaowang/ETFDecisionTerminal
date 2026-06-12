#include "MarketEngine/DisabledMarketDataProvider.h"

namespace etfdt::market_engine {

MarketProviderSnapshot DisabledMarketDataProvider::fetchReadOnly(
    const MarketRefreshInput& input)
{
    (void)input;
    MarketProviderSnapshot snapshot;
    snapshot.providerDisabled = true;
    snapshot.source = "disabled";
    snapshot.issues.push_back({
        "MARKET_DATA_PROVIDER_DISABLED",
        "provider",
        "Market data provider is disabled by default.",
        true,
    });
    return snapshot;
}

}  // namespace etfdt::market_engine
