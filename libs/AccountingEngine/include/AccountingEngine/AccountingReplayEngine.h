#pragma once

#include "AccountingEngine/AccountingReplayDtos.h"
#include "AccountingEngine/AccountingReplayResult.h"

#include <vector>

namespace etfdt::accounting {

class AccountingReplayEngine {
public:
    [[nodiscard]] AccountingReplayResult replayReadOnly(
        const ReplayRequestDto& request,
        const std::vector<TradeFactDto>& tradeFacts,
        const std::vector<CashFactDto>& cashFacts,
        const std::vector<MarketPriceFactDto>& marketPriceFacts,
        const std::vector<FxRateFactDto>& fxRateFacts) const;
};

[[nodiscard]] AccountingReplayResult replayReadOnly(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts);

} // namespace etfdt::accounting
