#pragma once

#include "CoreDomain/Result.h"

#include <string_view>

namespace etfdt::core {

enum class AccountType {
    Real,
    Simulated,
};

enum class InstrumentType {
    Etf,
    Index,
    OtcFund,
    Cash,
};

enum class MarketCode {
    CnA,
    Us,
    FundOtc,
    Cash,
};

enum class ActionType {
    Buy,
    Sell,
    Dividend,
    BonusShare,
    Split,
    Merge,
    CashIn,
    CashOut,
    CashAdjustment,
    CycleEvent,
    Correction,
    Reversal,
};

enum class TradeSource {
    ExchangeDirect,
    OtcSubstitute,
    Cash,
    System,
    Manual,
};

enum class TradeIntentType {
    BaseInit,
    BaseFill,
    BaseRepair,
    GridBuy,
    TakeProfit,
    PremiumSell,
    RiskSell,
    NormalBuy,
    NormalSell,
    CashEvent,
    CycleClose,
    Correction,
};

enum class DraftStatus {
    Active,
    Confirmed,
    Expired,
    Canceled,
    Superseded,
};

enum class MarketStatus {
    Realtime,
    Delayed,
    Stale,
    Failed,
    MarketClosed,
    FundNavPending,
    Unknown,
};

enum class GridCycleStatus {
    Active,
    Closed,
    Canceled,
};

enum class SuggestionStatus {
    Pending,
    Accepted,
    Rejected,
    Expired,
};

enum class LogLevel {
    Info,
    Warning,
    Error,
    Critical,
    Fatal,
};

[[nodiscard]] std::string_view toString(AccountType value) noexcept;
[[nodiscard]] std::string_view toString(InstrumentType value) noexcept;
[[nodiscard]] std::string_view toString(MarketCode value) noexcept;
[[nodiscard]] std::string_view toString(ActionType value) noexcept;
[[nodiscard]] std::string_view toString(TradeSource value) noexcept;
[[nodiscard]] std::string_view toString(TradeIntentType value) noexcept;
[[nodiscard]] std::string_view toString(DraftStatus value) noexcept;
[[nodiscard]] std::string_view toString(MarketStatus value) noexcept;
[[nodiscard]] std::string_view toString(GridCycleStatus value) noexcept;
[[nodiscard]] std::string_view toString(SuggestionStatus value) noexcept;
[[nodiscard]] std::string_view toString(LogLevel value) noexcept;

template <typename Enum>
[[nodiscard]] Result<Enum> fromString(std::string_view value);

template <>
[[nodiscard]] Result<AccountType> fromString<AccountType>(std::string_view value);

template <>
[[nodiscard]] Result<InstrumentType> fromString<InstrumentType>(std::string_view value);

template <>
[[nodiscard]] Result<MarketCode> fromString<MarketCode>(std::string_view value);

template <>
[[nodiscard]] Result<ActionType> fromString<ActionType>(std::string_view value);

template <>
[[nodiscard]] Result<TradeSource> fromString<TradeSource>(std::string_view value);

template <>
[[nodiscard]] Result<TradeIntentType> fromString<TradeIntentType>(std::string_view value);

template <>
[[nodiscard]] Result<DraftStatus> fromString<DraftStatus>(std::string_view value);

template <>
[[nodiscard]] Result<MarketStatus> fromString<MarketStatus>(std::string_view value);

template <>
[[nodiscard]] Result<GridCycleStatus> fromString<GridCycleStatus>(std::string_view value);

template <>
[[nodiscard]] Result<SuggestionStatus> fromString<SuggestionStatus>(std::string_view value);

template <>
[[nodiscard]] Result<LogLevel> fromString<LogLevel>(std::string_view value);

}  // namespace etfdt::core
