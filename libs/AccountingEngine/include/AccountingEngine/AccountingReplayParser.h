#pragma once

#include "AccountingEngine/AccountingIssue.h"
#include "AccountingEngine/AccountingReplayDtos.h"

#include <algorithm>
#include <string>
#include <vector>

namespace etfdt::accounting {

template <typename T>
struct AccountingParseResult {
    bool success = false;
    T value{};
    std::vector<AccountingIssueDto> issues;

    [[nodiscard]] bool hasIssueCode(const std::string& code) const
    {
        return std::any_of(issues.begin(), issues.end(), [&code](const AccountingIssueDto& issue) {
            return issue.code == code;
        });
    }
};

[[nodiscard]] AccountingParseResult<ReplayRequestDto> parseReplayRequestBoundary(const ReplayRequestDto& request);
[[nodiscard]] AccountingParseResult<TradeFactDto> parseTradeFactBoundary(const TradeFactDto& fact);
[[nodiscard]] AccountingParseResult<CashFactDto> parseCashFactBoundary(const CashFactDto& fact);
[[nodiscard]] AccountingParseResult<MarketPriceFactDto> parseMarketPriceFactBoundary(const MarketPriceFactDto& fact);
[[nodiscard]] AccountingParseResult<FxRateFactDto> parseFxRateFactBoundary(const FxRateFactDto& fact);

} // namespace etfdt::accounting
