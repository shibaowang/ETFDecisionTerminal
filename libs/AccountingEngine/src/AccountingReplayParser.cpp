#include "AccountingEngine/AccountingReplayParser.h"

#include "AccountingEngine/AccountingReplayValidation.h"

namespace etfdt::accounting {
namespace {

template <typename T>
AccountingParseResult<T> makeParseResult(const T& value, const AccountingValidationResult& validation)
{
    return AccountingParseResult<T>{
        validation.valid,
        value,
        validation.issues,
    };
}

} // namespace

AccountingParseResult<ReplayRequestDto> parseReplayRequestBoundary(const ReplayRequestDto& request)
{
    return makeParseResult(request, validateReplayRequest(request));
}

AccountingParseResult<TradeFactDto> parseTradeFactBoundary(const TradeFactDto& fact)
{
    return makeParseResult(fact, validateTradeFact(fact));
}

AccountingParseResult<CashFactDto> parseCashFactBoundary(const CashFactDto& fact)
{
    return makeParseResult(fact, validateCashFact(fact));
}

AccountingParseResult<MarketPriceFactDto> parseMarketPriceFactBoundary(const MarketPriceFactDto& fact)
{
    return makeParseResult(fact, validateMarketPriceFact(fact));
}

AccountingParseResult<FxRateFactDto> parseFxRateFactBoundary(const FxRateFactDto& fact)
{
    return makeParseResult(fact, validateFxRateFact(fact));
}

} // namespace etfdt::accounting
