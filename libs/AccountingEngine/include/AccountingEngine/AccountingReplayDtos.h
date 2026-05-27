#pragma once

#include <string>
#include <vector>

namespace etfdt::accounting {

namespace RequestedReplayOutput {
inline constexpr const char* Positions = "positions";
inline constexpr const char* Cash = "cash";
inline constexpr const char* Pnl = "pnl";
inline constexpr const char* BasePosition = "basePosition";
inline constexpr const char* SniperPool = "sniperPool";
inline constexpr const char* Issues = "issues";
} // namespace RequestedReplayOutput

namespace TradeAction {
inline constexpr const char* Buy = "BUY";
inline constexpr const char* Sell = "SELL";
} // namespace TradeAction

namespace CashAction {
inline constexpr const char* InitialCash = "INITIAL_CASH";
inline constexpr const char* Deposit = "DEPOSIT";
inline constexpr const char* Withdraw = "WITHDRAW";
inline constexpr const char* Adjustment = "ADJUSTMENT";
} // namespace CashAction

struct ReplayRequestDto {
    std::string accountId;
    std::string portfolioId;
    std::string sourceFromTime;
    std::string sourceToTime;
    std::vector<std::string> requestedOutputs;
    bool includeIssues = true;
    std::string calculationVersion;
};

struct TradeFactDto {
    std::string factId;
    std::string tradeTime;
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string action;
    std::string quantityText;
    std::string priceText;
    std::string amountText;
    std::string feeText;
    std::string cashFlowText;
    std::string currency;
    std::string source;
    std::string note;
};

struct CashFactDto {
    std::string factId;
    std::string time;
    std::string accountId;
    std::string portfolioId;
    std::string action;
    std::string amountText;
    std::string currency;
    std::string note;
};

struct MarketPriceFactDto {
    std::string instrumentCode;
    std::string priceTime;
    std::string priceText;
    std::string currency;
    std::string source;
};

struct FxRateFactDto {
    std::string baseCurrency;
    std::string quoteCurrency;
    std::string rateTime;
    std::string rateText;
    std::string source;
};

} // namespace etfdt::accounting
