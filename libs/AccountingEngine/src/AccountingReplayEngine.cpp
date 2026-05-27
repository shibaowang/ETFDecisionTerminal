#include "AccountingEngine/AccountingReplayEngine.h"

#include "AccountingEngine/AccountingReplayValidation.h"

namespace etfdt::accounting {

AccountingReplayResult AccountingReplayEngine::replayReadOnly(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts) const
{
    const auto validation = validateReplayRequest(request);
    if (!validation.valid) {
        return makeInvalidReplayRequestResult(validation.issues);
    }

    if (tradeFacts.empty() && cashFacts.empty() && marketPriceFacts.empty() && fxRateFacts.empty()) {
        return makeEmptyLedgerReplayResult(request.accountId, request.portfolioId);
    }

    return makeUnsupportedReplayScenarioResult();
}

AccountingReplayResult replayReadOnly(
    const ReplayRequestDto& request,
    const std::vector<TradeFactDto>& tradeFacts,
    const std::vector<CashFactDto>& cashFacts,
    const std::vector<MarketPriceFactDto>& marketPriceFacts,
    const std::vector<FxRateFactDto>& fxRateFacts)
{
    return AccountingReplayEngine{}.replayReadOnly(request, tradeFacts, cashFacts, marketPriceFacts, fxRateFacts);
}

} // namespace etfdt::accounting
