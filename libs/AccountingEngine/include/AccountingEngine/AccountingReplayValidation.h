#pragma once

#include "AccountingEngine/AccountingIssue.h"
#include "AccountingEngine/AccountingReplayDtos.h"

#include <string>
#include <vector>

namespace etfdt::accounting {

struct AccountingValidationResult {
    bool valid = true;
    std::vector<AccountingIssueDto> issues;

    [[nodiscard]] static AccountingValidationResult ok();
    void addIssue(const AccountingIssueDto& issue);
    [[nodiscard]] bool hasIssueCode(const std::string& code) const;
};

[[nodiscard]] bool isSupportedRequestedOutput(const std::string& text);
[[nodiscard]] bool isSupportedTradeAction(const std::string& text);
[[nodiscard]] bool isSupportedCashAction(const std::string& text);
[[nodiscard]] bool isSupportedIssueLevel(const std::string& text);
[[nodiscard]] bool looksLikeIsoTime(const std::string& text);
[[nodiscard]] bool looksLikeMoneyText(const std::string& text);
[[nodiscard]] bool looksLikeQuantityText(const std::string& text);
[[nodiscard]] std::string normalizeCurrencyCode(const std::string& text);
[[nodiscard]] bool isSupportedCurrencyCode(const std::string& text);

[[nodiscard]] AccountingValidationResult validateReplayRequest(const ReplayRequestDto& request);
[[nodiscard]] AccountingValidationResult validateTradeFact(const TradeFactDto& fact);
[[nodiscard]] AccountingValidationResult validateCashFact(const CashFactDto& fact);
[[nodiscard]] AccountingValidationResult validateMarketPriceFact(const MarketPriceFactDto& fact);
[[nodiscard]] AccountingValidationResult validateFxRateFact(const FxRateFactDto& fact);

} // namespace etfdt::accounting
