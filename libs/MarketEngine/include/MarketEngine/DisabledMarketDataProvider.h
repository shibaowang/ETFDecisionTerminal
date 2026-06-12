#pragma once

#include "MarketEngine/MarketDataProvider.h"

namespace etfdt::market_engine {

class DisabledMarketDataProvider final : public MarketDataProvider {
public:
    [[nodiscard]] MarketProviderSnapshot fetchReadOnly(
        const MarketRefreshInput& input) override;
};

}  // namespace etfdt::market_engine
