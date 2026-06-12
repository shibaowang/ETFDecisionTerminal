#pragma once

#include "MarketEngine/MarketDataProvider.h"

#include <string>

namespace etfdt::market_engine {

class FixtureMarketDataProvider final : public MarketDataProvider {
public:
    FixtureMarketDataProvider() = default;
    explicit FixtureMarketDataProvider(std::string fixturePayloadJson);

    [[nodiscard]] MarketProviderSnapshot fetchReadOnly(
        const MarketRefreshInput& input) override;

private:
    std::string fixturePayloadJson_;
};

}  // namespace etfdt::market_engine
