#pragma once

#include "AccountingEngine/AccountingIssue.h"

#include <string>
#include <vector>

namespace etfdt::accounting {

namespace ShellAccountingReplayStatus {
inline constexpr const char* Completed = "COMPLETED";
inline constexpr const char* Rejected = "REJECTED";
} // namespace ShellAccountingReplayStatus

struct ShellAccountingReplayTradeFact {
    std::string factId;
    std::string tradeTime;
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string instrumentType;
    std::string side;
    std::string quantityText;
    std::string priceText;
    std::string amountText;
    std::string feeText;
    std::string cashFlowText;
    std::string currency;
    std::string source;
    std::string note;
};

struct ShellAccountingReplayCashFact {
    std::string factId;
    std::string time;
    std::string accountId;
    std::string portfolioId;
    std::string action;
    std::string amountText;
    std::string currency;
    std::string note;
};

struct ShellAccountingReplayMarketPriceFact {
    std::string instrumentCode;
    std::string priceTime;
    std::string priceText;
    std::string currency;
    std::string source;
};

struct ShellAccountingReplayInput {
    std::string schemaVersion;
    std::vector<ShellAccountingReplayTradeFact> tradeFacts;
    std::vector<ShellAccountingReplayCashFact> cashFacts;
    std::vector<ShellAccountingReplayMarketPriceFact> marketPriceFacts;
};

struct ShellAccountingReplayPosition {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string instrumentType;
    std::string positionBucket;
    std::string quantityText;
    std::string averageCostText;
    std::string costBasisText;
    std::string marketValueText;
    std::string realizedPnlText;
    std::string unrealizedPnlText;
    std::string totalFeeText;
    std::string currency;
    std::string dataQualityStatus;
};

struct ShellAccountingReplayCashSummary {
    std::string accountId;
    std::string portfolioId;
    std::string currency;
    std::string cashBalanceText;
    std::string dataQualityStatus;
};

struct ShellAccountingReplayPnlSummary {
    std::string currency;
    std::string realizedPnlText;
    std::string unrealizedPnlText;
    std::string totalFeeText;
    std::string totalMarketValueText;
    std::string dataQualityStatus;
};

struct ShellAccountingReplayResult {
    bool accepted = false;
    bool replayExecuted = false;
    bool readOnly = true;
    std::string status;
    std::vector<AccountingIssueDto> issues;
    std::vector<ShellAccountingReplayPosition> positions;
    std::vector<ShellAccountingReplayCashSummary> cashSummaries;
    ShellAccountingReplayPnlSummary pnlSummary;
};

class ShellAccountingPortfolioReplayEngine {
public:
    [[nodiscard]] ShellAccountingReplayResult replayReadOnly(
        const ShellAccountingReplayInput& input) const;
};

[[nodiscard]] ShellAccountingReplayResult replayShellAccountingPortfolioReadOnly(
    const ShellAccountingReplayInput& input);

} // namespace etfdt::accounting
