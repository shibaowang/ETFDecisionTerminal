#include "CoreDomain/CoreDomain.h"

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(std::int64_t actual, std::int64_t expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

template <typename Enum>
void expectEnumRoundTrip(Enum value, std::string_view text, std::string_view message)
{
    using etfdt::core::fromString;
    using etfdt::core::toString;

    expectEqual(toString(value), text, message);
    auto parsed = fromString<Enum>(text);
    expectTrue(parsed.hasValue(), std::string(message) + " parse should succeed");
    if (parsed.hasValue()) {
        expectTrue(parsed.value() == value, std::string(message) + " parsed enum mismatch");
    }
}

void testMoneyCents()
{
    using etfdt::core::MoneyCents;

    expectEqual(MoneyCents{}.raw(), 0, "MoneyCents default is zero");
    static_assert(!std::is_convertible_v<MoneyCents, double>);

    auto zero = MoneyCents::fromYuanString("0");
    expectTrue(zero.hasValue(), "MoneyCents parses 0");
    expectEqual(zero.value().raw(), 0, "MoneyCents 0 raw");

    auto one = MoneyCents::fromYuanString("1");
    expectTrue(one.hasValue(), "MoneyCents parses 1");
    expectEqual(one.value().raw(), 100, "MoneyCents 1 raw");

    auto oneTwentyThree = MoneyCents::fromYuanString("1.23");
    expectTrue(oneTwentyThree.hasValue(), "MoneyCents parses 1.23");
    expectEqual(oneTwentyThree.value().raw(), 123, "MoneyCents 1.23 raw");
    expectEqual(oneTwentyThree.value().toYuanString(), "1.23", "MoneyCents toYuanString");

    auto onePointTwo = MoneyCents::fromYuanString("1.2");
    expectTrue(onePointTwo.hasValue(), "MoneyCents parses 1.2");
    expectEqual(onePointTwo.value().raw(), 120, "MoneyCents 1.2 raw");

    auto negative = MoneyCents::fromYuanString("-1.23");
    expectTrue(negative.hasValue(), "MoneyCents parses -1.23");
    expectEqual(negative.value().raw(), -123, "MoneyCents -1.23 raw");
    expectEqual(negative.value().toYuanString(), "-1.23", "MoneyCents negative toYuanString");

    auto invalid = MoneyCents::fromYuanString("abc");
    expectTrue(!invalid.hasValue(), "MoneyCents rejects abc");
    expectTrue(!invalid.error().empty(), "MoneyCents invalid parse has error");

    expectTrue(oneTwentyThree.value() > onePointTwo.value(), "MoneyCents comparison");
    expectEqual((one.value() + onePointTwo.value()).raw(), 220, "MoneyCents addition");
    expectEqual((one.value() - onePointTwo.value()).raw(), -20, "MoneyCents subtraction");
}

void testPriceQuantityRatio()
{
    using etfdt::core::Price1e6;
    using etfdt::core::Quantity1e6;
    using etfdt::core::RatioPpm;

    auto price = Price1e6::fromDecimalString("1.234567");
    expectTrue(price.hasValue(), "Price1e6 parses 1.234567");
    expectEqual(price.value().raw(), 1234567, "Price1e6 raw");
    expectEqual(price.value().toDecimalString(), "1.234567", "Price1e6 toDecimalString");

    auto priceShort = Price1e6::fromDecimalString("1.2");
    expectTrue(priceShort.hasValue(), "Price1e6 parses 1.2");
    expectEqual(priceShort.value().raw(), 1200000, "Price1e6 1.2 raw");
    expectEqual(priceShort.value().toDecimalString(), "1.2", "Price1e6 1.2 string");

    auto quantity = Quantity1e6::fromDecimalString("100.000001");
    expectTrue(quantity.hasValue(), "Quantity1e6 parses 100.000001");
    expectEqual(quantity.value().raw(), 100000001, "Quantity1e6 raw");
    expectEqual(
        quantity.value().toDecimalString(),
        "100.000001",
        "Quantity1e6 toDecimalString");

    auto ratio = RatioPpm::fromPercentString("20%");
    expectTrue(ratio.hasValue(), "RatioPpm parses 20%");
    expectEqual(ratio.value().raw(), 200000, "RatioPpm 20% raw");
    expectEqual(ratio.value().toPercentString(), "20%", "RatioPpm 20% string");

    auto ratioSmall = RatioPpm::fromPercentString("0.05%");
    expectTrue(ratioSmall.hasValue(), "RatioPpm parses 0.05%");
    expectEqual(ratioSmall.value().raw(), 500, "RatioPpm 0.05% raw");
    expectEqual(ratioSmall.value().toPercentString(), "0.05%", "RatioPpm 0.05% string");
}

void testEnums()
{
    using namespace etfdt::core;

    expectEnumRoundTrip(AccountType::Real, "REAL", "AccountType REAL");
    expectEnumRoundTrip(AccountType::Simulated, "SIMULATED", "AccountType SIMULATED");

    expectEnumRoundTrip(InstrumentType::Etf, "ETF", "InstrumentType ETF");
    expectEnumRoundTrip(InstrumentType::Index, "INDEX", "InstrumentType INDEX");
    expectEnumRoundTrip(InstrumentType::OtcFund, "OTC_FUND", "InstrumentType OTC_FUND");
    expectEnumRoundTrip(InstrumentType::Cash, "CASH", "InstrumentType CASH");

    expectEnumRoundTrip(MarketCode::CnA, "CN_A", "MarketCode CN_A");
    expectEnumRoundTrip(MarketCode::Us, "US", "MarketCode US");
    expectEnumRoundTrip(MarketCode::FundOtc, "FUND_OTC", "MarketCode FUND_OTC");
    expectEnumRoundTrip(MarketCode::Cash, "CASH", "MarketCode CASH");

    expectEnumRoundTrip(ActionType::Buy, "BUY", "ActionType BUY");
    expectEnumRoundTrip(ActionType::Sell, "SELL", "ActionType SELL");
    expectEnumRoundTrip(ActionType::Dividend, "DIVIDEND", "ActionType DIVIDEND");
    expectEnumRoundTrip(ActionType::BonusShare, "BONUS_SHARE", "ActionType BONUS_SHARE");
    expectEnumRoundTrip(ActionType::Split, "SPLIT", "ActionType SPLIT");
    expectEnumRoundTrip(ActionType::Merge, "MERGE", "ActionType MERGE");
    expectEnumRoundTrip(ActionType::CashIn, "CASH_IN", "ActionType CASH_IN");
    expectEnumRoundTrip(ActionType::CashOut, "CASH_OUT", "ActionType CASH_OUT");
    expectEnumRoundTrip(
        ActionType::CashAdjustment,
        "CASH_ADJUSTMENT",
        "ActionType CASH_ADJUSTMENT");
    expectEnumRoundTrip(ActionType::CycleEvent, "CYCLE_EVENT", "ActionType CYCLE_EVENT");
    expectEnumRoundTrip(ActionType::Correction, "CORRECTION", "ActionType CORRECTION");
    expectEnumRoundTrip(ActionType::Reversal, "REVERSAL", "ActionType REVERSAL");

    expectEnumRoundTrip(
        TradeSource::ExchangeDirect,
        "EXCHANGE_DIRECT",
        "TradeSource EXCHANGE_DIRECT");
    expectEnumRoundTrip(
        TradeSource::OtcSubstitute,
        "OTC_SUBSTITUTE",
        "TradeSource OTC_SUBSTITUTE");
    expectEnumRoundTrip(TradeSource::Cash, "CASH", "TradeSource CASH");
    expectEnumRoundTrip(TradeSource::System, "SYSTEM", "TradeSource SYSTEM");
    expectEnumRoundTrip(TradeSource::Manual, "MANUAL", "TradeSource MANUAL");

    expectEnumRoundTrip(TradeIntentType::BaseInit, "BASE_INIT", "TradeIntentType BASE_INIT");
    expectEnumRoundTrip(TradeIntentType::BaseFill, "BASE_FILL", "TradeIntentType BASE_FILL");
    expectEnumRoundTrip(
        TradeIntentType::BaseRepair,
        "BASE_REPAIR",
        "TradeIntentType BASE_REPAIR");
    expectEnumRoundTrip(TradeIntentType::GridBuy, "GRID_BUY", "TradeIntentType GRID_BUY");
    expectEnumRoundTrip(
        TradeIntentType::TakeProfit,
        "TAKE_PROFIT",
        "TradeIntentType TAKE_PROFIT");
    expectEnumRoundTrip(
        TradeIntentType::PremiumSell,
        "PREMIUM_SELL",
        "TradeIntentType PREMIUM_SELL");
    expectEnumRoundTrip(TradeIntentType::RiskSell, "RISK_SELL", "TradeIntentType RISK_SELL");
    expectEnumRoundTrip(
        TradeIntentType::NormalBuy,
        "NORMAL_BUY",
        "TradeIntentType NORMAL_BUY");
    expectEnumRoundTrip(
        TradeIntentType::NormalSell,
        "NORMAL_SELL",
        "TradeIntentType NORMAL_SELL");
    expectEnumRoundTrip(TradeIntentType::CashEvent, "CASH_EVENT", "TradeIntentType CASH_EVENT");
    expectEnumRoundTrip(
        TradeIntentType::CycleClose,
        "CYCLE_CLOSE",
        "TradeIntentType CYCLE_CLOSE");
    expectEnumRoundTrip(TradeIntentType::Correction, "CORRECTION", "TradeIntentType CORRECTION");

    expectEnumRoundTrip(DraftStatus::Active, "ACTIVE", "DraftStatus ACTIVE");
    expectEnumRoundTrip(DraftStatus::Confirmed, "CONFIRMED", "DraftStatus CONFIRMED");
    expectEnumRoundTrip(DraftStatus::Expired, "EXPIRED", "DraftStatus EXPIRED");
    expectEnumRoundTrip(DraftStatus::Canceled, "CANCELED", "DraftStatus CANCELED");
    expectEnumRoundTrip(DraftStatus::Superseded, "SUPERSEDED", "DraftStatus SUPERSEDED");

    expectEnumRoundTrip(MarketStatus::Realtime, "REALTIME", "MarketStatus REALTIME");
    expectEnumRoundTrip(MarketStatus::Delayed, "DELAYED", "MarketStatus DELAYED");
    expectEnumRoundTrip(MarketStatus::Stale, "STALE", "MarketStatus STALE");
    expectEnumRoundTrip(MarketStatus::Failed, "FAILED", "MarketStatus FAILED");
    expectEnumRoundTrip(
        MarketStatus::MarketClosed,
        "MARKET_CLOSED",
        "MarketStatus MARKET_CLOSED");
    expectEnumRoundTrip(
        MarketStatus::FundNavPending,
        "FUND_NAV_PENDING",
        "MarketStatus FUND_NAV_PENDING");
    expectEnumRoundTrip(MarketStatus::Unknown, "UNKNOWN", "MarketStatus UNKNOWN");

    expectEnumRoundTrip(GridCycleStatus::Active, "ACTIVE", "GridCycleStatus ACTIVE");
    expectEnumRoundTrip(GridCycleStatus::Closed, "CLOSED", "GridCycleStatus CLOSED");
    expectEnumRoundTrip(GridCycleStatus::Canceled, "CANCELED", "GridCycleStatus CANCELED");

    expectEnumRoundTrip(SuggestionStatus::Pending, "PENDING", "SuggestionStatus PENDING");
    expectEnumRoundTrip(SuggestionStatus::Accepted, "ACCEPTED", "SuggestionStatus ACCEPTED");
    expectEnumRoundTrip(SuggestionStatus::Rejected, "REJECTED", "SuggestionStatus REJECTED");
    expectEnumRoundTrip(SuggestionStatus::Expired, "EXPIRED", "SuggestionStatus EXPIRED");

    expectEnumRoundTrip(LogLevel::Info, "INFO", "LogLevel INFO");
    expectEnumRoundTrip(LogLevel::Warning, "WARNING", "LogLevel WARNING");
    expectEnumRoundTrip(LogLevel::Error, "ERROR", "LogLevel ERROR");
    expectEnumRoundTrip(LogLevel::Critical, "CRITICAL", "LogLevel CRITICAL");
    expectEnumRoundTrip(LogLevel::Fatal, "FATAL", "LogLevel FATAL");

    auto invalid = fromString<AccountType>("UNKNOWN");
    expectTrue(!invalid.hasValue(), "Enum fromString rejects unknown value");
}

void testIdentifiers()
{
    using etfdt::core::InstrumentCode;
    using etfdt::core::StrategyCode;
    using etfdt::core::Uuid;

    Uuid uuid(" 00000000-0000-4000-8000-000000000001 ");
    expectTrue(uuid.isValidBasicFormat(), "Uuid basic non-empty validation");
    expectEqual(uuid.raw(), "00000000-0000-4000-8000-000000000001", "Uuid trim");

    auto strategyCode = StrategyCode::fromString(" ETF_QDII ");
    expectTrue(strategyCode.hasValue(), "StrategyCode non-empty");
    expectEqual(strategyCode.value().raw(), "ETF_QDII", "StrategyCode trim");

    auto emptyStrategyCode = StrategyCode::fromString("  ");
    expectTrue(!emptyStrategyCode.hasValue(), "StrategyCode rejects empty string");

    auto instrumentCode = InstrumentCode::fromString(" 513500 ");
    expectTrue(instrumentCode.hasValue(), "InstrumentCode non-empty");
    expectEqual(instrumentCode.value().raw(), "513500", "InstrumentCode trim");

    auto emptyInstrumentCode = InstrumentCode::fromString("");
    expectTrue(!emptyInstrumentCode.hasValue(), "InstrumentCode rejects empty string");
}

}  // namespace

int main()
{
    testMoneyCents();
    testPriceQuantityRatio();
    testEnums();
    testIdentifiers();

    if (gFailures != 0) {
        std::cerr << gFailures << " CoreDomain test failure(s)" << '\n';
        return 1;
    }

    std::cout << "CoreDomain tests passed." << '\n';
    return 0;
}
