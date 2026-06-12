#pragma once

#include <string>
#include <vector>

namespace etfdt::strategy_engine {

struct StrategyRecommendationBuyThreshold final {
    std::string tierLabel;
    std::string drawdownText;
    std::string targetAmountText;
};

struct StrategyRecommendationInput final {
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string instrumentType;
    std::string strategyCode;
    std::string tradeSource;
    std::string currentPriceText;
    std::string otcNavText;
    std::string marketPriceText;
    std::string indexDrawdownText;
    std::string premiumText;
    std::string returnRateText;
    std::string principalBaseText;
    std::string holdingCostText;
    std::string positionQuantityText;
    std::string positionAverageCostText;
    std::string cashBalanceText;
    std::string targetGridAmountText;
    std::string cumulativeTierRemainingText;
    std::string sellReturnThresholdText;
    std::string takeProfitPremiumThresholdText;
    std::string extremePremiumThresholdText;
    std::string basePositionRatioText;
    std::string feeRateText;
    std::string fixedFeeText;
    std::string requestedSellQuantityText;
    std::string memo;
    int minLotSize = 100;
    std::vector<StrategyRecommendationBuyThreshold> buyThresholds;
};

struct StrategyRecommendationIssue final {
    std::string level = "ERROR";
    std::string code;
    std::string message;
    std::string field;
    bool blocking = true;
};

struct StrategyRecommendationResult final {
    std::string status = "REJECTED";
    std::string actionCode = "REJECTED";
    std::string actionLabel = "Rejected";
    std::string sourceCode;
    std::string sourceLabel;
    std::string reasonCode = "INPUT_REJECTED";
    std::string reasonLabel = "Input rejected";
    std::string tierLabel;
    std::string targetAmountText = "0.00";
    std::string suggestedQuantityText = "0";
    std::string suggestedAmountText = "0.00";
    std::string netCashImpactText = "0.00";
    std::string feeText = "0.00";
    bool accepted = false;
    bool recommendationComputed = false;
    bool baseProtectionPassed = true;
    bool cashLimitApplied = false;
    bool readOnlyNoWrite = true;
    bool tradeDraftCreated = false;
    bool tradeLogRowsWrittenByRecommendation = false;
    bool cashAdjustmentRowsWrittenByRecommendation = false;
    bool auditLogRowsWrittenByRecommendation = false;
    bool productionDbTouched = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool brokerOrderSubmitted = false;
    bool automaticTrading = false;
    std::vector<StrategyRecommendationIssue> issues;
};

struct TradeDraftConversionRequest final {
    StrategyRecommendationResult recommendation;
    std::string accountId;
    std::string portfolioId;
    std::string strategyCode;
    std::string instrumentCode;
    std::string instrumentType;
    std::string tradeSource;
    std::string idempotencyKey;
    std::string recommendationDigest;
    std::string userNote;
    bool userConfirmed = false;
};

struct TradeDraftConversionResult final {
    std::string status = "REJECTED";
    std::string side;
    std::string sourceRecommendationAction;
    std::string sourceRecommendationReason;
    std::string instrumentCode;
    std::string instrumentType;
    std::string strategyCode;
    std::string tradeSource;
    std::string quantityText = "0";
    std::string amountText = "0.00";
    std::string priceText = "0.00";
    std::string feeEstimateText = "0.00";
    std::string netCashImpactText = "0.00";
    std::string currency = "CNY";
    bool eligible = false;
    bool userConfirmationRequired = true;
    bool userConfirmed = false;
    bool baseProtectionPassed = true;
    bool cashLimitApplied = false;
    bool otcGenericDraft = false;
    bool multiChannelOtcLegsCreated = false;
    bool tradeDraftIsNotOrder = true;
    bool noWriteInConversion = true;
    std::vector<StrategyRecommendationIssue> issues;
    std::vector<std::string> issueCodes;
};

class StrategyRecommendationEngine final {
public:
    [[nodiscard]] StrategyRecommendationResult recommendReadOnly(
        const StrategyRecommendationInput& input) const;
    [[nodiscard]] TradeDraftConversionResult convertRecommendationToTradeDraft(
        const TradeDraftConversionRequest& request) const;
};

[[nodiscard]] bool decodeStrategyRecommendationInputJson(
    const std::string& payloadJson,
    StrategyRecommendationInput& input,
    std::string& errorCode);

}  // namespace etfdt::strategy_engine
