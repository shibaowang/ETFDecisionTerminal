#include "CoreDomain/Enums.h"

#include <array>
#include <string>

namespace etfdt::core {
namespace {

template <typename Enum, std::size_t Size>
std::string_view enumToString(
    Enum value,
    const std::array<std::pair<Enum, std::string_view>, Size>& entries) noexcept
{
    for (const auto& entry : entries) {
        if (entry.first == value) {
            return entry.second;
        }
    }
    return {};
}

template <typename Enum, std::size_t Size>
Result<Enum> enumFromString(
    std::string_view value,
    const std::array<std::pair<Enum, std::string_view>, Size>& entries,
    std::string_view enumName)
{
    for (const auto& entry : entries) {
        if (entry.second == value) {
            return Result<Enum>::success(entry.first);
        }
    }
    return Result<Enum>::failure("Unknown " + std::string(enumName) + ": " + std::string(value));
}

constexpr std::array<std::pair<AccountType, std::string_view>, 2> kAccountTypes = {
    {{AccountType::Real, "REAL"}, {AccountType::Simulated, "SIMULATED"}}};

constexpr std::array<std::pair<InstrumentType, std::string_view>, 4> kInstrumentTypes = {
    {{InstrumentType::Etf, "ETF"},
     {InstrumentType::Index, "INDEX"},
     {InstrumentType::OtcFund, "OTC_FUND"},
     {InstrumentType::Cash, "CASH"}}};

constexpr std::array<std::pair<MarketCode, std::string_view>, 4> kMarketCodes = {
    {{MarketCode::CnA, "CN_A"},
     {MarketCode::Us, "US"},
     {MarketCode::FundOtc, "FUND_OTC"},
     {MarketCode::Cash, "CASH"}}};

constexpr std::array<std::pair<ActionType, std::string_view>, 12> kActionTypes = {
    {{ActionType::Buy, "BUY"},
     {ActionType::Sell, "SELL"},
     {ActionType::Dividend, "DIVIDEND"},
     {ActionType::BonusShare, "BONUS_SHARE"},
     {ActionType::Split, "SPLIT"},
     {ActionType::Merge, "MERGE"},
     {ActionType::CashIn, "CASH_IN"},
     {ActionType::CashOut, "CASH_OUT"},
     {ActionType::CashAdjustment, "CASH_ADJUSTMENT"},
     {ActionType::CycleEvent, "CYCLE_EVENT"},
     {ActionType::Correction, "CORRECTION"},
     {ActionType::Reversal, "REVERSAL"}}};

constexpr std::array<std::pair<TradeSource, std::string_view>, 5> kTradeSources = {
    {{TradeSource::ExchangeDirect, "EXCHANGE_DIRECT"},
     {TradeSource::OtcSubstitute, "OTC_SUBSTITUTE"},
     {TradeSource::Cash, "CASH"},
     {TradeSource::System, "SYSTEM"},
     {TradeSource::Manual, "MANUAL"}}};

constexpr std::array<std::pair<TradeIntentType, std::string_view>, 12> kTradeIntentTypes = {
    {{TradeIntentType::BaseInit, "BASE_INIT"},
     {TradeIntentType::BaseFill, "BASE_FILL"},
     {TradeIntentType::BaseRepair, "BASE_REPAIR"},
     {TradeIntentType::GridBuy, "GRID_BUY"},
     {TradeIntentType::TakeProfit, "TAKE_PROFIT"},
     {TradeIntentType::PremiumSell, "PREMIUM_SELL"},
     {TradeIntentType::RiskSell, "RISK_SELL"},
     {TradeIntentType::NormalBuy, "NORMAL_BUY"},
     {TradeIntentType::NormalSell, "NORMAL_SELL"},
     {TradeIntentType::CashEvent, "CASH_EVENT"},
     {TradeIntentType::CycleClose, "CYCLE_CLOSE"},
     {TradeIntentType::Correction, "CORRECTION"}}};

constexpr std::array<std::pair<DraftStatus, std::string_view>, 5> kDraftStatuses = {
    {{DraftStatus::Active, "ACTIVE"},
     {DraftStatus::Confirmed, "CONFIRMED"},
     {DraftStatus::Expired, "EXPIRED"},
     {DraftStatus::Canceled, "CANCELED"},
     {DraftStatus::Superseded, "SUPERSEDED"}}};

constexpr std::array<std::pair<MarketStatus, std::string_view>, 7> kMarketStatuses = {
    {{MarketStatus::Realtime, "REALTIME"},
     {MarketStatus::Delayed, "DELAYED"},
     {MarketStatus::Stale, "STALE"},
     {MarketStatus::Failed, "FAILED"},
     {MarketStatus::MarketClosed, "MARKET_CLOSED"},
     {MarketStatus::FundNavPending, "FUND_NAV_PENDING"},
     {MarketStatus::Unknown, "UNKNOWN"}}};

constexpr std::array<std::pair<GridCycleStatus, std::string_view>, 3> kGridCycleStatuses = {
    {{GridCycleStatus::Active, "ACTIVE"},
     {GridCycleStatus::Closed, "CLOSED"},
     {GridCycleStatus::Canceled, "CANCELED"}}};

constexpr std::array<std::pair<SuggestionStatus, std::string_view>, 4> kSuggestionStatuses = {
    {{SuggestionStatus::Pending, "PENDING"},
     {SuggestionStatus::Accepted, "ACCEPTED"},
     {SuggestionStatus::Rejected, "REJECTED"},
     {SuggestionStatus::Expired, "EXPIRED"}}};

constexpr std::array<std::pair<LogLevel, std::string_view>, 5> kLogLevels = {
    {{LogLevel::Info, "INFO"},
     {LogLevel::Warning, "WARNING"},
     {LogLevel::Error, "ERROR"},
     {LogLevel::Critical, "CRITICAL"},
     {LogLevel::Fatal, "FATAL"}}};

}  // namespace

std::string_view toString(AccountType value) noexcept
{
    return enumToString(value, kAccountTypes);
}

std::string_view toString(InstrumentType value) noexcept
{
    return enumToString(value, kInstrumentTypes);
}

std::string_view toString(MarketCode value) noexcept
{
    return enumToString(value, kMarketCodes);
}

std::string_view toString(ActionType value) noexcept
{
    return enumToString(value, kActionTypes);
}

std::string_view toString(TradeSource value) noexcept
{
    return enumToString(value, kTradeSources);
}

std::string_view toString(TradeIntentType value) noexcept
{
    return enumToString(value, kTradeIntentTypes);
}

std::string_view toString(DraftStatus value) noexcept
{
    return enumToString(value, kDraftStatuses);
}

std::string_view toString(MarketStatus value) noexcept
{
    return enumToString(value, kMarketStatuses);
}

std::string_view toString(GridCycleStatus value) noexcept
{
    return enumToString(value, kGridCycleStatuses);
}

std::string_view toString(SuggestionStatus value) noexcept
{
    return enumToString(value, kSuggestionStatuses);
}

std::string_view toString(LogLevel value) noexcept
{
    return enumToString(value, kLogLevels);
}

template <>
Result<AccountType> fromString<AccountType>(std::string_view value)
{
    return enumFromString(value, kAccountTypes, "AccountType");
}

template <>
Result<InstrumentType> fromString<InstrumentType>(std::string_view value)
{
    return enumFromString(value, kInstrumentTypes, "InstrumentType");
}

template <>
Result<MarketCode> fromString<MarketCode>(std::string_view value)
{
    return enumFromString(value, kMarketCodes, "MarketCode");
}

template <>
Result<ActionType> fromString<ActionType>(std::string_view value)
{
    return enumFromString(value, kActionTypes, "ActionType");
}

template <>
Result<TradeSource> fromString<TradeSource>(std::string_view value)
{
    return enumFromString(value, kTradeSources, "TradeSource");
}

template <>
Result<TradeIntentType> fromString<TradeIntentType>(std::string_view value)
{
    return enumFromString(value, kTradeIntentTypes, "TradeIntentType");
}

template <>
Result<DraftStatus> fromString<DraftStatus>(std::string_view value)
{
    return enumFromString(value, kDraftStatuses, "DraftStatus");
}

template <>
Result<MarketStatus> fromString<MarketStatus>(std::string_view value)
{
    return enumFromString(value, kMarketStatuses, "MarketStatus");
}

template <>
Result<GridCycleStatus> fromString<GridCycleStatus>(std::string_view value)
{
    return enumFromString(value, kGridCycleStatuses, "GridCycleStatus");
}

template <>
Result<SuggestionStatus> fromString<SuggestionStatus>(std::string_view value)
{
    return enumFromString(value, kSuggestionStatuses, "SuggestionStatus");
}

template <>
Result<LogLevel> fromString<LogLevel>(std::string_view value)
{
    return enumFromString(value, kLogLevels, "LogLevel");
}

}  // namespace etfdt::core
