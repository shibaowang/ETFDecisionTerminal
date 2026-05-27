#pragma once

#include "AccountingEngine/AccountingIssue.h"

#include <string>
#include <vector>

namespace etfdt::accounting {

namespace AccountingReplayStatus {
inline constexpr const char* Ok = "OK";
inline constexpr const char* Warning = "WARNING";
inline constexpr const char* NotImplemented = "NOT_IMPLEMENTED";
inline constexpr const char* InvalidRequest = "INVALID_REQUEST";
inline constexpr const char* UnsupportedScenario = "UNSUPPORTED_SCENARIO";
inline constexpr const char* Error = "ERROR";
} // namespace AccountingReplayStatus

struct PositionSummaryDto {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string quantityText;
    std::string costAmountText;
    std::string costPriceText;
    std::string currency;
    std::string dataQualityStatus;
};

struct PositionListResponseDto {
    std::vector<PositionSummaryDto> positions;
    std::string dataQualityStatus;
};

struct CashSummaryDto {
    std::string accountId;
    std::string portfolioId;
    std::string currency;
    std::string cashBalanceText;
    std::string dataQualityStatus;
};

struct PortfolioPnlDto {
    std::string portfolioId;
    std::string currency;
    std::string realizedPnlText;
    std::string unrealizedPnlText;
    std::string totalAssetsText;
    std::string totalPnlText;
    std::string dataQualityStatus;
};

struct BasePositionDto {
    std::string dataQualityStatus;
};

struct SniperPoolDto {
    std::string dataQualityStatus;
};

struct AccountingReplayResult {
    bool implemented = false;
    bool replayExecuted = false;
    std::string status;
    std::string message;
    std::vector<AccountingIssueDto> issues;
    PositionListResponseDto positionList;
    bool hasCashSummary = false;
    CashSummaryDto cashSummary;
    bool hasPortfolioPnl = false;
    PortfolioPnlDto portfolioPnl;
    bool hasBasePosition = false;
    BasePositionDto basePosition;
    bool hasSniperPool = false;
    SniperPoolDto sniperPool;
};

[[nodiscard]] AccountingReplayResult makeEmptyLedgerReplayResult(
    const std::string& accountId,
    const std::string& portfolioId);

[[nodiscard]] AccountingReplayResult makeSingleBuyReplayResult(
    const std::string& accountId,
    const std::string& portfolioId,
    const std::string& instrumentCode,
    const std::string& quantityText,
    long long costCents,
    long long cashBalanceCents);

[[nodiscard]] AccountingReplayResult makeBuySellPartialReplayResult(
    const std::string& accountId,
    const std::string& portfolioId,
    const std::string& instrumentCode,
    const std::string& remainingQuantityText,
    long long remainingCostCents,
    long long cashBalanceCents,
    long long realizedPnlCents);

[[nodiscard]] AccountingReplayResult makeInvalidReplayRequestResult(std::vector<AccountingIssueDto> issues);
[[nodiscard]] AccountingReplayResult makeUnsupportedReplayScenarioResult();
[[nodiscard]] AccountingReplayResult makeNegativeCashReplayResult();
[[nodiscard]] AccountingReplayResult makeSellExceedsPositionReplayResult();
[[nodiscard]] AccountingReplayResult makeMissingFeeReplayResult();

} // namespace etfdt::accounting
